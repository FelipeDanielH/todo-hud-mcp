import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

@Injectable()
export class ListTasksUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(): Promise<Task[]> {
    return this.repo.findAll();
  }
}
