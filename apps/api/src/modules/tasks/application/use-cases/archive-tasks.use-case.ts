import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';
import { TaskEventsPublisher } from '../services/task-events-publisher';

@Injectable()
export class ArchiveTasksUseCase {
  constructor(
    @Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort,
    private readonly events: TaskEventsPublisher,
  ) {}

  async execute(phaseId?: string): Promise<{ archived: number; archivedAt: string }> {
    const all = await this.repo.findAll();
    const now = new Date().toISOString();

    const toArchive = phaseId
      ? all.filter((t) => t.status === 'completed' && t.phaseId === phaseId)
      : all.filter((t) => t.status === 'completed');

    for (const task of toArchive) {
      const archived = task.archive(now);
      await this.repo.update(task.id, archived);
    }

    await this.events.tasksArchived(toArchive.length, phaseId);
    return { archived: toArchive.length, archivedAt: now };
  }
}
