export const EVENT_BUS = 'EVENT_BUS';

export type TaskEventReason =
  | 'task.created'
  | 'task.batch_created'
  | 'task.completed'
  | 'task.reopened'
  | 'task.archived'
  | 'tasks.archived'
  | 'tasks.changed';

export interface TaskEvent {
  type: 'tasks.changed';
  version: number;
  reason: TaskEventReason;
  taskId?: string;
  phaseId?: string;
  timestamp: string;
}

export interface EventBusPort {
  publish(event: TaskEvent): Promise<void>;
  subscribe(handler: (event: TaskEvent) => void): () => void;
}
