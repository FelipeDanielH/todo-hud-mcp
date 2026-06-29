import * as fs from 'fs';
import * as path from 'path';
import { Task, backfillTask } from '../../domain/models/task';
import { TasksRepositoryPort } from '../../application/ports/tasks-repository.port';

export class JsonTasksRepository implements TasksRepositoryPort {
  private readonly filePath: string;
  private tasks: Task[] = [];

  constructor(dataDir?: string) {
    const dir = dataDir ?? path.resolve(process.cwd(), 'data');
    if (!fs.existsSync(dir)) {
      fs.mkdirSync(dir, { recursive: true });
    }
    this.filePath = path.join(dir, 'tasks.json');
    this.load();
  }

  private load(): void {
    try {
      if (fs.existsSync(this.filePath)) {
        const raw = fs.readFileSync(this.filePath, 'utf-8');
        const arr: Record<string, unknown>[] = JSON.parse(raw);

        const needsMigration = arr.some(
          (t) => t.status === undefined || t.sortOrder === undefined,
        );

        if (needsMigration) {
          this.backup();
        }

        this.tasks = arr.map((t, i) => {
          const task = backfillTask(t);
          if (task.sortOrder === undefined) {
            return new Task(
              task.id,
              task.title,
              task.description,
              task.completed,
              task.priority,
              task.createdAt,
              task.updatedAt,
              task.completedAt,
              task.status,
              task.phaseId,
              task.phaseName,
              task.archivedAt,
              i,
            );
          }
          return task;
        });

        if (needsMigration) {
          this.persist();
        }
      } else {
        this.tasks = [];
        this.persist();
      }
    } catch {
      this.tasks = [];
      this.persist();
    }
  }

  private backup(): void {
    try {
      const now = new Date();
      const ts =
        now.getFullYear().toString() +
        String(now.getMonth() + 1).padStart(2, '0') +
        String(now.getDate()).padStart(2, '0') + '-' +
        String(now.getHours()).padStart(2, '0') +
        String(now.getMinutes()).padStart(2, '0') +
        String(now.getSeconds()).padStart(2, '0');
      const bakPath = this.filePath + '.bak-' + ts;
      fs.copyFileSync(this.filePath, bakPath);
      console.log('Backup created:', bakPath);
    } catch (err) {
      console.warn('Backup failed, continuing:', err);
    }
  }

  private persist(): void {
    fs.writeFileSync(this.filePath, JSON.stringify(this.tasks, null, 2), 'utf-8');
  }

  async findAll(): Promise<Task[]> {
    return [...this.tasks];
  }

  async findById(id: string): Promise<Task | null> {
    return this.tasks.find((t) => t.id === id) ?? null;
  }

  async findByPhase(phaseId: string): Promise<Task[]> {
    return this.tasks.filter((t) => t.phaseId === phaseId);
  }

  async findArchived(): Promise<Task[]> {
    return this.tasks.filter((t) => t.status === 'archived');
  }

  async save(task: Task): Promise<Task> {
    this.tasks.push(task);
    this.persist();
    return task;
  }

  async saveAll(tasks: Task[]): Promise<Task[]> {
    this.tasks.push(...tasks);
    this.persist();
    return tasks;
  }

  async update(
    id: string,
    data: Partial<Omit<Task, 'id' | 'createdAt'>>,
  ): Promise<Task | null> {
    const index = this.tasks.findIndex((t) => t.id === id);
    if (index === -1) return null;

    const existing = this.tasks[index];
    const updated = new Task(
      existing.id,
      data.title ?? existing.title,
      data.description !== undefined ? data.description : existing.description,
      data.completed ?? existing.completed,
      data.priority !== undefined ? data.priority : existing.priority,
      existing.createdAt,
      new Date().toISOString(),
      data.completedAt !== undefined ? data.completedAt : existing.completedAt,
      data.status ?? existing.status,
      data.phaseId !== undefined ? data.phaseId : existing.phaseId,
      data.phaseName !== undefined ? data.phaseName : existing.phaseName,
      data.archivedAt !== undefined ? data.archivedAt : existing.archivedAt,
      data.sortOrder !== undefined ? data.sortOrder : existing.sortOrder,
    );
    this.tasks[index] = updated;
    this.persist();
    return updated;
  }

  async delete(id: string): Promise<boolean> {
    const index = this.tasks.findIndex((t) => t.id === id);
    if (index === -1) return false;
    this.tasks.splice(index, 1);
    this.persist();
    return true;
  }
}
