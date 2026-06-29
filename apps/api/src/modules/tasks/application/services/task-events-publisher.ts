import { Injectable, Inject } from '@nestjs/common';
import { EVENT_BUS, EventBusPort, TaskEvent, TaskEventReason } from '../ports/event-bus.port';
import { Task } from '../../domain/models';

@Injectable()
export class TaskEventsPublisher {
  constructor(@Inject(EVENT_BUS) private readonly eventBus: EventBusPort) {}

  async taskCreated(task: Task): Promise<void> {
    await this.safePublish({ reason: 'task.created', taskId: task.id, phaseId: task.phaseId });
  }

  async taskBatchCreated(tasks: Task[], phaseId?: string): Promise<void> {
    await this.safePublish({ reason: 'task.batch_created', phaseId });
  }

  async taskCompleted(taskId: string, phaseId?: string): Promise<void> {
    await this.safePublish({ reason: 'task.completed', taskId, phaseId });
  }

  async taskReopened(taskId: string, phaseId?: string): Promise<void> {
    await this.safePublish({ reason: 'task.reopened', taskId, phaseId });
  }

  async tasksArchived(count: number, phaseId?: string): Promise<void> {
    await this.safePublish({ reason: 'tasks.archived', phaseId });
  }

  private async safePublish(data: {
    reason: TaskEventReason;
    taskId?: string;
    phaseId?: string;
  }): Promise<void> {
    try {
      await this.eventBus.publish({
        type: 'tasks.changed',
        version: 1,
        ...data,
        timestamp: new Date().toISOString(),
      });
    } catch (error) {
      console.error('[TaskEventsPublisher] Failed to publish event:', error);
    }
  }
}
