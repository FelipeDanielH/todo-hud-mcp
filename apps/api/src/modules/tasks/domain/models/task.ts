import { Priority } from './priority';

export type TaskStatus = 'pending' | 'completed' | 'archived';

export class Task {
  constructor(
    public readonly id: string,
    public title: string,
    public readonly description: string | undefined,
    public completed: boolean,
    public readonly priority: Priority | undefined,
    public readonly createdAt: string,
    public updatedAt: string,
    public completedAt: string | undefined,
    public status: TaskStatus,
    public readonly phaseId: string | undefined,
    public readonly phaseName: string | undefined,
    public archivedAt: string | undefined,
    public readonly sortOrder: number | undefined,
  ) {}

  static create(data: {
    id: string;
    title: string;
    description?: string;
    priority?: Priority;
    phaseId?: string;
    phaseName?: string;
    sortOrder?: number;
  }): Task {
    const now = new Date().toISOString();
    return new Task(
      data.id,
      data.title,
      data.description,
      false,
      data.priority,
      now,
      now,
      undefined,
      'pending',
      data.phaseId,
      data.phaseName,
      undefined,
      data.sortOrder,
    );
  }

  complete(): Task {
    const now = new Date().toISOString();
    return new Task(
      this.id,
      this.title,
      this.description,
      true,
      this.priority,
      this.createdAt,
      now,
      now,
      'completed',
      this.phaseId,
      this.phaseName,
      undefined,
      this.sortOrder,
    );
  }

  reopen(): Task {
    return new Task(
      this.id,
      this.title,
      this.description,
      false,
      this.priority,
      this.createdAt,
      new Date().toISOString(),
      undefined,
      'pending',
      this.phaseId,
      this.phaseName,
      undefined,
      this.sortOrder,
    );
  }

  archive(archivedAt: string): Task {
    return new Task(
      this.id,
      this.title,
      this.description,
      true,
      this.priority,
      this.createdAt,
      new Date().toISOString(),
      this.completedAt,
      'archived',
      this.phaseId,
      this.phaseName,
      archivedAt,
      this.sortOrder,
    );
  }
}

export function backfillTask(raw: Record<string, unknown>): Task {
  const completed = Boolean(raw.completed);
  const completedAt = raw.completedAt as string | undefined;
  return new Task(
    raw.id as string,
    raw.title as string,
    raw.description as string | undefined,
    completed,
    raw.priority as Priority | undefined,
    raw.createdAt as string,
    raw.updatedAt as string,
    completedAt,
    (raw.status as TaskStatus) ?? (completed ? 'completed' : 'pending'),
    raw.phaseId as string | undefined,
    raw.phaseName as string | undefined,
    raw.archivedAt as string | undefined,
    raw.sortOrder != null ? Number(raw.sortOrder) : undefined,
  );
}
