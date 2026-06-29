import {
  WebSocketGateway,
  WebSocketServer,
  OnGatewayConnection,
  OnGatewayDisconnect,
} from '@nestjs/websockets';
import { WebSocket, Server } from 'ws';
import { Inject, OnModuleInit, OnModuleDestroy, Logger } from '@nestjs/common';
import { EVENT_BUS, EventBusPort, TaskEvent } from '../../application/ports/event-bus.port';

@WebSocketGateway({
  path: '/ws/tasks',
  cors: { origin: '*' },
})
export class RealtimeGateway
  implements OnGatewayConnection, OnGatewayDisconnect, OnModuleInit, OnModuleDestroy
{
  private readonly logger = new Logger(RealtimeGateway.name);
  private clients = new Set<WebSocket>();
  private unsubscribe: () => void = () => {};

  @WebSocketServer()
  server!: Server;

  constructor(@Inject(EVENT_BUS) private readonly eventBus: EventBusPort) {}

  onModuleInit(): void {
    this.unsubscribe = this.eventBus.subscribe((event) => this.broadcast(event));
    this.logger.log('RealtimeGateway initialized, subscribed to EventBus');
  }

  onModuleDestroy(): void {
    this.unsubscribe?.();
    this.logger.log('RealtimeGateway destroyed, unsubscribed from EventBus');
  }

  handleConnection(client: WebSocket): void {
    this.clients.add(client);
    this.logger.log(`WebSocket client connected (${this.clients.size} total)`);

    client.on('close', () => {
      this.clients.delete(client);
      this.logger.log(`WebSocket client disconnected (${this.clients.size} total)`);
    });

    client.on('error', (err) => {
      this.logger.error(`WebSocket client error: ${err.message}`);
      this.clients.delete(client);
    });
  }

  handleDisconnect(client: WebSocket): void {
    this.clients.delete(client);
  }

  private broadcast(event: TaskEvent): void {
    if (this.clients.size === 0) return;

    const message = JSON.stringify(event);
    for (const client of this.clients) {
      if (client.readyState === WebSocket.OPEN) {
        try {
          client.send(message);
        } catch (err) {
          this.logger.error(`Failed to send message to client: ${err instanceof Error ? err.message : String(err)}`);
          this.clients.delete(client);
        }
      }
    }
  }
}
