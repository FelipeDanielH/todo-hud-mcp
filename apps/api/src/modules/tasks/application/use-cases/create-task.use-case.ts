import { Injectable, Inject } from '@nestjs/common';
import { randomUUID } from 'crypto';
import { Task, Priority } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';
import { TaskEventsPublisher } from '../services/task-events-publisher';

@Injectable()
export class CreateTaskUseCase {
  constructor(
    @Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort,
    private readonly events: TaskEventsPublisher,
  ) {}

  async execute(data: { title: string; description?: string; priority?: Priority }): Promise<Task> {
    const task = Task.create({ id: randomUUID(), ...data });
    const saved = await this.repo.save(task);
    await this.events.taskCreated(saved);
    return saved;
  }
}
