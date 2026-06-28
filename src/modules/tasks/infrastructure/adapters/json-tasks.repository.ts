import * as fs from 'fs';
import * as path from 'path';
import { Task } from '../../domain/models/task';
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
        const arr: Task[] = JSON.parse(raw);
        this.tasks = arr.map(
          (t) =>
            new Task(
              t.id,
              t.title,
              t.description,
              t.completed,
              t.priority,
              t.createdAt,
              t.updatedAt,
              t.completedAt,
            ),
        );
      } else {
        this.tasks = [];
        this.persist();
      }
    } catch {
      this.tasks = [];
      this.persist();
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

  async save(task: Task): Promise<Task> {
    this.tasks.push(task);
    this.persist();
    return task;
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
