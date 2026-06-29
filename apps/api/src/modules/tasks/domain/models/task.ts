import { Priority } from './priority';

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
  ) {}

  static create(data: {
    id: string;
    title: string;
    description?: string;
    priority?: Priority;
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
    );
  }
}
