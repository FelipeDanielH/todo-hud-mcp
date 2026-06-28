import { Injectable, Inject } from '@nestjs/common';
import { randomUUID } from 'crypto';
import { Task, Priority } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

@Injectable()
export class CreateTaskUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(data: { title: string; description?: string; priority?: Priority }): Promise<Task> {
    const task = Task.create({ id: randomUUID(), ...data });
    return this.repo.save(task);
  }
}
