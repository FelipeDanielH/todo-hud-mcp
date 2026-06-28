import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TaskNotFoundError } from '../../domain/exceptions';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

@Injectable()
export class ReopenTaskUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(id: string): Promise<Task> {
    const task = await this.repo.findById(id);
    if (!task) throw new TaskNotFoundError(id);

    const reopened = task.reopen();
    const saved = await this.repo.update(id, reopened);
    if (!saved) throw new TaskNotFoundError(id);
    return saved;
  }
}
