import { Task } from '../../domain/models/task';

export const TASKS_REPOSITORY_PORT = 'TASKS_REPOSITORY_PORT';

export interface TasksRepositoryPort {
  findAll(): Promise<Task[]>;
  findById(id: string): Promise<Task | null>;
  save(task: Task): Promise<Task>;
  update(id: string, data: Partial<Omit<Task, 'id' | 'createdAt'>>): Promise<Task | null>;
  delete(id: string): Promise<boolean>;
}
