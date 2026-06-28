import {
  Controller,
  Post,
  Body,
  Res,
  Req,
  HttpCode,
  HttpStatus,
  Headers,
  Logger,
} from '@nestjs/common';
import {
  ApiTags,
  ApiOperation,
  ApiResponse,
  ApiBody,
} from '@nestjs/swagger';
import { Request, Response } from 'express';
import { McpHandlerService } from './mcp.handler.service';

interface JsonRpcRequest {
  jsonrpc: string;
  method: string;
  id?: string | number;
  params?: Record<string, unknown>;
}

interface JsonRpcSuccessResponse {
  jsonrpc: string;
  id: string | number;
  result: unknown;
}

interface JsonRpcErrorResponse {
  jsonrpc: string;
  id: string | number | null;
  error: { code: number; message: string; data?: unknown };
}

// POST /mcp body schema for OpenAPI docs
class McpRequestBody {
  jsonrpc!: string;
  method!: string;
  id?: string | number;
  params?: Record<string, unknown>;
}

@ApiTags('MCP')
@Controller()
export class McpController {
  private readonly logger = new Logger(McpController.name);

  constructor(private readonly mcpHandler: McpHandlerService) {}

  @Post('mcp')
  @HttpCode(HttpStatus.OK)
  @ApiOperation({
    summary: 'MCP Streamable HTTP endpoint',
    description:
      'Accepts JSON-RPC 2.0 messages per the Model Context Protocol. ' +
      'Supports initialize, tools/list, tools/call, and notifications. ' +
      'Returns JSON responses for immediate results. ' +
      'Set Accept: text/event-stream for SSE streaming responses.',
  })
  @ApiBody({ type: McpRequestBody })
  @ApiResponse({ status: 200, description: 'JSON-RPC response' })
  @ApiResponse({ status: 202, description: 'Notification accepted (no response body)' })
  @ApiResponse({ status: 400, description: 'Parse error or invalid request' })
  async handleMcp(
    @Body() body: unknown,
    @Res() res: Response,
    @Req() req: Request,
    @Headers('accept') accept?: string,
  ): Promise<void> {
    const wantsSse: boolean = accept?.includes('text/event-stream') ?? false;

    try {
      const msg = this.validateMessage(body);
      if (!msg) {
        this.sendError(res, wantsSse, null, -32600, 'Invalid Request: body must be a valid JSON-RPC 2.0 message');
        return;
      }

      const result = await this.processMessage(msg);

      if (result === null) {
        // Notification — no response body
        res.status(HttpStatus.ACCEPTED).end();
        return;
      }

      this.sendResult(res, wantsSse, msg.id!, result);
    } catch (err: unknown) {
      const rpcErr = err as { isRpcError?: boolean; rpcCode?: number; message?: string };
      if (rpcErr.isRpcError) {
        this.logger.warn(`MCP RPC error [${rpcErr.rpcCode}]: ${rpcErr.message}`);
        if (!res.headersSent) {
          const msg = body as { id?: string | number } | undefined;
          this.sendError(res, wantsSse, msg?.id ?? null, rpcErr.rpcCode ?? -32603, rpcErr.message ?? 'Unknown error');
        }
      } else {
        this.logger.error('Unhandled MCP error', err);
        if (!res.headersSent) {
          const msg = body as { id?: string | number } | undefined;
          this.sendError(res, wantsSse, msg?.id ?? null, -32603, 'Internal error');
        }
      }
    }
  }

  private validateMessage(body: unknown): JsonRpcRequest | null {
    if (!body || typeof body !== 'object') return null;
    const msg = body as Record<string, unknown>;
    if (msg.jsonrpc !== '2.0') return null;
    if (typeof msg.method !== 'string' || msg.method.length === 0) return null;
    return msg as unknown as JsonRpcRequest;
  }

  private async processMessage(
    msg: JsonRpcRequest,
  ): Promise<unknown | null> {
    const { method, params } = msg;

    // Notification — no id field
    if (msg.id === undefined || msg.id === null) {
      return null;
    }

    switch (method) {
      case 'initialize':
        return this.mcpHandler.getInitializeResult();

      case 'notifications/initialized':
        return null;

      case 'tools/list':
        return { tools: this.mcpHandler.getToolList() };

      case 'tools/call': {
        const p = (params ?? {}) as { name?: string; arguments?: Record<string, unknown> };
        if (!p.name || typeof p.name !== 'string') {
          throw this.rpcError(-32602, 'Invalid params: name is required');
        }
        const toolResult = await this.mcpHandler.handleToolCall(
          p.name,
          p.arguments ?? {},
        );
        return {
          content: toolResult.content,
          isError: toolResult.isError ?? false,
        };
      }

      default:
        throw this.rpcError(-32601, `Method not found: ${method}`);
    }
  }

  private sendResult(
    res: Response,
    wantsSse: boolean,
    id: string | number,
    result: unknown,
  ): void {
    const response: JsonRpcSuccessResponse = {
      jsonrpc: '2.0',
      id,
      result,
    };

    if (wantsSse) {
      res.setHeader('Content-Type', 'text/event-stream');
      res.setHeader('Cache-Control', 'no-cache');
      res.setHeader('Connection', 'keep-alive');
      res.write(`event: message\ndata: ${JSON.stringify(response)}\n\n`);
      res.end();
    } else {
      res.json(response);
    }
  }

  private sendError(
    res: Response,
    wantsSse: boolean,
    id: string | number | null,
    code: number,
    message: string,
    data?: unknown,
  ): void {
    const response: JsonRpcErrorResponse = {
      jsonrpc: '2.0',
      id,
      error: { code, message, data },
    };

    if (wantsSse) {
      res.setHeader('Content-Type', 'text/event-stream');
      res.setHeader('Cache-Control', 'no-cache');
      res.setHeader('Connection', 'keep-alive');
      res.write(`event: message\ndata: ${JSON.stringify(response)}\n\n`);
      res.end();
    } else {
      res.status(HttpStatus.OK).json(response);
    }
  }

  private rpcError(code: number, message: string): never {
    throw Object.assign(new Error(message), { rpcCode: code, isRpcError: true });
  }
}
