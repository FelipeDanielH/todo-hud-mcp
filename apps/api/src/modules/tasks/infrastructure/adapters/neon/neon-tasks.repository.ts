import { Task } from '../../../domain/models/task';
import { TasksRepositoryPort } from '../../../application/ports/tasks-repository.port';

/**
 * Adaptador PostgreSQL vía NeonDB para TasksRepositoryPort.
 *
 * Pendiente de implementar cuando se migre la persistencia.
 * Requisitos:
 *   - @neondatabase/serverless (o @prisma/client con datasource neon)
 *   - Variable de entorno DATABASE_URL con conexión a NeonDB
 *   - Migración SQL: CREATE TABLE tasks (...)
 *
 * @example
 * ```ts
 * // tasks.module.ts
 * {
 *   provide: TASKS_REPOSITORY_PORT,
 *   useFactory: () => new NeonTasksRepository(process.env.DATABASE_URL!),
 * }
 * ```
 */
export class NeonTasksRepository implements TasksRepositoryPort {
  constructor(private readonly connectionString: string) {
    // TODO: inicializar pool de conexión NeonDB
    // const { neon } = await import('@neondatabase/serverless');
    // this.sql = neon(connectionString);
  }

  async findAll(): Promise<Task[]> {
    throw new Error('NeonTasksRepository#findAll not implemented. Use JsonTasksRepository for now.');
  }

  async findById(_id: string): Promise<Task | null> {
    throw new Error('NeonTasksRepository#findById not implemented. Use JsonTasksRepository for now.');
  }

  async findByPhase(_phaseId: string): Promise<Task[]> {
    throw new Error('NeonTasksRepository#findByPhase not implemented. Use JsonTasksRepository for now.');
  }

  async findArchived(): Promise<Task[]> {
    throw new Error('NeonTasksRepository#findArchived not implemented. Use JsonTasksRepository for now.');
  }

  async save(_task: Task): Promise<Task> {
    throw new Error('NeonTasksRepository#save not implemented. Use JsonTasksRepository for now.');
  }

  async saveAll(_tasks: Task[]): Promise<Task[]> {
    throw new Error('NeonTasksRepository#saveAll not implemented. Use JsonTasksRepository for now.');
  }

  async update(_id: string, _data: Partial<Omit<Task, 'id' | 'createdAt'>>): Promise<Task | null> {
    throw new Error('NeonTasksRepository#update not implemented. Use JsonTasksRepository for now.');
  }

  async delete(_id: string): Promise<boolean> {
    throw new Error('NeonTasksRepository#delete not implemented. Use JsonTasksRepository for now.');
  }
}
