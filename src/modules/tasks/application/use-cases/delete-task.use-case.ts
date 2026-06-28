import { Injectable, Inject } from '@nestjs/common';
import { TaskNotFoundError } from '../../domain/exceptions';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

@Injectable()
export class DeleteTaskUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(id: string): Promise<void> {
    const task = await this.repo.findById(id);
    if (!task) throw new TaskNotFoundError(id);

    const deleted = await this.repo.delete(id);
    if (!deleted) throw new TaskNotFoundError(id);
  }
}
