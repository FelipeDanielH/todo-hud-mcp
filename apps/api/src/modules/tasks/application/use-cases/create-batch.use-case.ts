import { Injectable, Inject } from '@nestjs/common';
import { randomUUID } from 'crypto';
import { Task, Priority } from '../../domain/models';
import { TasksRepositoryPort, TASKS_REPOSITORY_PORT } from '../ports/tasks-repository.port';
import { TaskEventsPublisher } from '../services/task-events-publisher';

@Injectable()
export class CreateBatchUseCase {
  constructor(
    @Inject(TASKS_REPOSITORY_PORT) private readonly repo: TasksRepositoryPort,
    private readonly events: TaskEventsPublisher,
  ) {}

  async execute(data: { phaseName: string; tasks: string[] }): Promise<{
    phaseId: string;
    phaseName: string;
    tasks: Task[];
  }> {
    const phaseId = randomUUID();
    const tasks = data.tasks.map((title, i) =>
      Task.create({
        id: randomUUID(),
        title,
        phaseId,
        phaseName: data.phaseName,
        sortOrder: i,
      }),
    );
    await this.repo.saveAll(tasks);
    await this.events.taskBatchCreated(tasks, phaseId);
    return { phaseId, phaseName: data.phaseName, tasks };
  }
}
