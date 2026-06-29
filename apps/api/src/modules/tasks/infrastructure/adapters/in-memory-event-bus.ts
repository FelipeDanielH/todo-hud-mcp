import { EventBusPort, TaskEvent } from '../../application/ports/event-bus.port';

export class InMemoryEventBus implements EventBusPort {
  private handlers = new Set<(event: TaskEvent) => void>();

  async publish(event: TaskEvent): Promise<void> {
    for (const handler of this.handlers) {
      try {
        handler(event);
      } catch (err) {
        console.error('[InMemoryEventBus] Handler error:', err);
      }
    }
  }

  subscribe(handler: (event: TaskEvent) => void): () => void {
    this.handlers.add(handler);
    return () => {
      this.handlers.delete(handler);
    };
  }
}
