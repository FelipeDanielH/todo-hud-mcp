import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TaskNotFoundError } from '../../domain/exceptions';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';
import { TaskEventsPublisher } from '../services/task-events-publisher';

@Injectable()
export class CompleteTaskUseCase {
  constructor(
    @Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort,
    private readonly events: TaskEventsPublisher,
  ) {}

  async execute(id: string): Promise<Task> {
    const task = await this.repo.findById(id);
    if (!task) throw new TaskNotFoundError(id);

    const completed = task.complete();
    const saved = await this.repo.update(id, {
      completed: true,
      status: 'completed',
      completedAt: completed.completedAt,
      updatedAt: completed.updatedAt,
    });
    if (!saved) throw new TaskNotFoundError(id);
    await this.events.taskCompleted(id, task.phaseId);
    return saved;
  }
}
