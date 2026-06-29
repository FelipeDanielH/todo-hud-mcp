import { Injectable, Inject } from '@nestjs/common';
import { Task } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';

export interface ArchivedPhase {
  phaseId: string;
  phaseName: string;
  archivedAt: string;
  taskCount: number;
  tasks: Task[];
}

@Injectable()
export class ListArchivedUseCase {
  constructor(@Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort) {}

  async execute(): Promise<ArchivedPhase[]> {
    const archived = await this.repo.findArchived();

    const grouped = new Map<string, Task[]>();
    for (const task of archived) {
      const key = task.phaseId ?? '__orphaned__';
      if (!grouped.has(key)) grouped.set(key, []);
      grouped.get(key)!.push(task);
    }

    const phases: ArchivedPhase[] = [];
    for (const [phaseId, tasks] of grouped) {
      const name = phaseId === '__orphaned__'
        ? 'Sin fase'
        : tasks[0].phaseName ?? 'Sin nombre';
      const archivedAt = tasks.reduce(
        (latest, t) => (t.archivedAt && t.archivedAt > latest ? t.archivedAt : latest),
        '',
      );
      phases.push({ phaseId, phaseName: name, archivedAt, taskCount: tasks.length, tasks });
    }

    phases.sort((a, b) => b.archivedAt.localeCompare(a.archivedAt));
    return phases;
  }
}
