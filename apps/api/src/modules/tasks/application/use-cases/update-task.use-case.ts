import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TaskNotFoundError } from '../../domain/exceptions';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

@Injectable()
export class UpdateTaskUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(
    id: string,
    data: { title?: string; description?: string; priority?: string },
  ): Promise<Task> {
    const exists = await this.repo.findById(id);
    if (!exists) throw new TaskNotFoundError(id);

    const updated = await this.repo.update(id, data as Partial<Task>);
    if (!updated) throw new TaskNotFoundError(id);
    return updated;
  }
}
