import { Injectable } from '@nestjs/common';
import { CreateTaskUseCase } from '../../application/use-cases/create-task.use-case';
import { ListTasksUseCase } from '../../application/use-cases/list-tasks.use-case';

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
