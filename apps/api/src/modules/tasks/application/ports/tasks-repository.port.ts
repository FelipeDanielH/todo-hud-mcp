import { Task } from '../../domain/models/task';
import { BaseRepository } from './base.repository';

export const TASKS_REPOSITORY_PORT = 'TASKS_REPOSITORY_PORT';

export interface TasksRepositoryPort extends BaseRepository<Task> {
  findAll(): Promise<Task[]>;
  findById(id: string): Promise<Task | null>;
  findByPhase(phaseId: string): Promise<Task[]>;
  findArchived(): Promise<Task[]>;
  save(task: Task): Promise<Task>;
  saveAll(tasks: Task[]): Promise<Task[]>;
  update(id: string, data: Partial<Omit<Task, 'id' | 'createdAt'>>): Promise<Task | null>;
  delete(id: string): Promise<boolean>;
}
