import { Injectable } from '@nestjs/common';
import { CreateTaskUseCase } from '../../application/use-cases/create-task.use-case';
import { ListTasksUseCase } from '../../application/use-cases/list-tasks.use-case';
import { CompleteTaskUseCase } from '../../application/use-cases/complete-task.use-case';
import { CreateBatchUseCase } from '../../application/use-cases/create-batch.use-case';
import { ArchiveTasksUseCase } from '../../application/use-cases/archive-tasks.use-case';
import { TaskNotFoundError } from '../../domain/exceptions/task-not-found.error';

export interface McpToolContent {
  type: 'text';
  text: string;
}

export interface McpToolResult {
  content: McpToolContent[];
  isError?: boolean;
}

export interface McpToolDefinition {
  name: string;
  description?: string;
  inputSchema: Record<string, unknown>;
  handler: (args: Record<string, unknown>) => Promise<McpToolResult>;
}

export interface ServerCapabilities {
  tools?: Record<string, unknown>;
  resources?: Record<string, unknown>;
  prompts?: Record<string, unknown>;
}

export interface InitializeResult {
  protocolVersion: string;
  capabilities: ServerCapabilities;
  serverInfo: { name: string; version: string };
}

export interface ToolListItem {
  name: string;
  description?: string;
  inputSchema: Record<string, unknown>;
}

@Injectable()
export class McpHandlerService {
  private readonly tools: McpToolDefinition[];

  constructor(
    private readonly createTaskUseCase: CreateTaskUseCase,
    private readonly listTasksUseCase: ListTasksUseCase,
    private readonly completeTaskUseCase: CompleteTaskUseCase,
    private readonly createBatchUseCase: CreateBatchUseCase,
    private readonly archiveTasksUseCase: ArchiveTasksUseCase,
  ) {
    this.tools = this.buildTools();
  }

  private buildTools(): McpToolDefinition[] {
    return [
      {
        name: 'set_tasks',
        description: 'Create multiple tasks from a list of titles',
        inputSchema: {
          type: 'object',
          properties: {
            titles: {
              type: 'array',
              items: { type: 'string' },
              description: 'List of task titles to create',
            },
          },
          required: ['titles'],
        },
        handler: async (args) => {
          const titles = args.titles as unknown;
          if (!Array.isArray(titles) || titles.length === 0) {
            return {
              content: [
                { type: 'text', text: 'titles must be a non-empty array of strings' },
              ],
              isError: true,
            };
          }
          const created = await Promise.all(
            titles.map((title: string) =>
              this.createTaskUseCase.execute({ title }),
            ),
          );
          return {
            content: [
              {
                type: 'text',
                text: JSON.stringify(
                  { count: created.length, tasks: created },
                  null,
                  2,
                ),
              },
            ],
          };
        },
      },
      {
        name: 'create_batch',
        description: 'Create a batch of tasks as a named phase',
        inputSchema: {
          type: 'object',
          properties: {
            phaseName: {
              type: 'string',
              description: 'Name of the phase (e.g. "Lab 281 - Fase 1")',
            },
            tasks: {
              type: 'array',
              items: { type: 'string' },
              description: 'List of task titles for this phase',
            },
          },
          required: ['phaseName', 'tasks'],
        },
        handler: async (args) => {
          const phaseName = args.phaseName as unknown;
          const tasks = args.tasks as unknown;
          if (typeof phaseName !== 'string' || !Array.isArray(tasks) || tasks.length === 0) {
            return {
              content: [
                { type: 'text', text: 'phaseName must be a string and tasks a non-empty array' },
              ],
              isError: true,
            };
          }
          const result = await this.createBatchUseCase.execute({
            phaseName,
            tasks: tasks as string[],
          });
          return {
            content: [
              {
                type: 'text',
                text: JSON.stringify(result, null, 2),
              },
            ],
          };
        },
      },
      {
        name: 'list_tasks',
        description: 'Retrieve all existing tasks',
        inputSchema: { type: 'object', properties: {} },
        handler: async () => {
          const tasks = await this.listTasksUseCase.execute();
          return {
            content: [
              {
                type: 'text',
                text: JSON.stringify(
                  { count: tasks.length, tasks },
                  null,
                  2,
                ),
              },
            ],
          };
        },
      },
      {
        name: 'complete_task',
        description: 'Mark an existing task as completed by its ID',
        inputSchema: {
          type: 'object',
          properties: {
            id: {
              type: 'string',
              description: 'ID of the task to complete',
            },
          },
          required: ['id'],
        },
        handler: async (args) => {
          const id = args.id as unknown;
          if (!id || typeof id !== 'string') {
            return {
              content: [{ type: 'text', text: 'id must be a non-empty string' }],
              isError: true,
            };
          }
          try {
            const updated = await this.completeTaskUseCase.execute(id);
            return {
              content: [
                {
                  type: 'text',
                  text: JSON.stringify(updated, null, 2),
                },
              ],
            };
          } catch (error) {
            if (error instanceof TaskNotFoundError) {
              return {
                content: [{ type: 'text', text: `Task not found: ${id}` }],
                isError: true,
              };
            }
            throw error;
          }
        },
      },
      {
        name: 'archive_tasks',
        description: 'Archive all completed tasks, optionally filtered by phaseId',
        inputSchema: {
          type: 'object',
          properties: {
            phaseId: {
              type: 'string',
              description: 'Optional phase ID to archive only tasks in that phase',
            },
          },
        },
        handler: async (args) => {
          const phaseId = args.phaseId as string | undefined;
          const result = await this.archiveTasksUseCase.execute(phaseId);
          return {
            content: [
              {
                type: 'text',
                text: JSON.stringify(result, null, 2),
              },
            ],
          };
        },
      },
    ];
  }

  getToolList(): ToolListItem[] {
    return this.tools.map((t) => ({
      name: t.name,
      description: t.description,
      inputSchema: t.inputSchema,
    }));
  }

  getInitializeResult(): InitializeResult {
    return {
      protocolVersion: '2025-03-26',
      capabilities: { tools: {} },
      serverInfo: { name: 'focus-hud', version: '0.1.0' },
    };
  }

  async handleToolCall(
    name: string,
    args: Record<string, unknown>,
  ): Promise<McpToolResult> {
    const tool = this.tools.find((t) => t.name === name);
    if (!tool) {
      return {
        content: [{ type: 'text', text: `Unknown tool: ${name}` }],
        isError: true,
      };
    }
    return tool.handler(args);
  }
}
