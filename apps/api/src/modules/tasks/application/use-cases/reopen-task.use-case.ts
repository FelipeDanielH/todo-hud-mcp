import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TaskNotFoundError } from '../../domain/exceptions';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';
import { TaskEventsPublisher } from '../services/task-events-publisher';

@Injectable()
export class ReopenTaskUseCase {
  constructor(
    @Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort,
    private readonly events: TaskEventsPublisher,
  ) {}

  async execute(id: string): Promise<Task> {
    const task = await this.repo.findById(id);
    if (!task) throw new TaskNotFoundError(id);

    const reopened = task.reopen();
    const saved = await this.repo.update(id, {
      completed: false,
      status: 'pending',
      completedAt: undefined,
      updatedAt: reopened.updatedAt,
    });
    if (!saved) throw new TaskNotFoundError(id);
    await this.events.taskReopened(id, task.phaseId);
    return saved;
  }
}
