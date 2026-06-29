import { McpServer } from '@modelcontextprotocol/server';
import { StdioServerTransport } from '@modelcontextprotocol/server/stdio';
import * as z from 'zod/v4';
import { JsonTasksRepository } from '../adapters/json-tasks.repository';
import { CreateTaskUseCase } from '../../application/use-cases/create-task.use-case';
import { ListTasksUseCase } from '../../application/use-cases/list-tasks.use-case';
import { CompleteTaskUseCase } from '../../application/use-cases/complete-task.use-case';
import { CreateBatchUseCase } from '../../application/use-cases/create-batch.use-case';
import { ArchiveTasksUseCase } from '../../application/use-cases/archive-tasks.use-case';
import { McpHandlerService } from './mcp.handler.service';

async function main(): Promise<void> {
  const dataPath = process.env.DATA_PATH;
  const repo = new JsonTasksRepository(dataPath);
  const handler = new McpHandlerService(
    new CreateTaskUseCase(repo),
    new ListTasksUseCase(repo),
    new CompleteTaskUseCase(repo),
    new CreateBatchUseCase(repo),
    new ArchiveTasksUseCase(repo),
  );

  const server = new McpServer({
    name: 'focus-hud',
    version: '0.1.0',
  });

  for (const tool of handler.getToolList()) {
    const zodProps: Record<string, z.ZodType<unknown>> = {};
    if (tool.inputSchema.properties) {
      const props = tool.inputSchema.properties as Record<string, { type?: string }>;
      for (const [key, val] of Object.entries(props)) {
        if (val.type === 'array') {
          zodProps[key] = z.array(z.string()).min(1);
        } else if (val.type === 'object') {
          zodProps[key] = z.object({}).optional();
        } else {
          zodProps[key] = z.string().optional();
        }
      }
    }

    server.registerTool(
      tool.name,
      {
        title: tool.name
          .replace(/_/g, ' ')
          .replace(/\b\w/g, (c) => c.toUpperCase()),
        description: tool.description,
        inputSchema: z.object(zodProps),
      },
      async (args) => {
        const result = await handler.handleToolCall(
          tool.name,
          args as Record<string, unknown>,
        );
        return {
          content: result.content.map((c) => ({
            type: c.type as 'text',
            text: c.text,
          })),
        };
      },
    );
  }

  const transport = new StdioServerTransport();
  await server.connect(transport);
  console.error('Focus HUD MCP server running on stdio');
}

main().catch((error) => {
  console.error('Fatal MCP server error:', error);
  process.exit(1);
});
