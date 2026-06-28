import { JsonTasksRepository } from './json-tasks.repository';
import { Task } from '../../domain/models/task';
import * as fs from 'fs';
import * as path from 'path';

describe('JsonTasksRepository', () => {
  const testDir = path.resolve(__dirname, '../../../../../data-test-hex');
  let repo: JsonTasksRepository;

  beforeEach(() => {
    if (fs.existsSync(testDir)) {
      fs.rmSync(testDir, { recursive: true, force: true });
    }
    repo = new JsonTasksRepository(testDir);
  });

  afterAll(() => {
    if (fs.existsSync(testDir)) {
      fs.rmSync(testDir, { recursive: true, force: true });
    }
  });

  it('should create and persist a task', async () => {
    const task = Task.create({ id: '1', title: 'Test task' });
    const created = await repo.save(task);
    expect(created.id).toBe('1');
    expect(created.title).toBe('Test task');
    expect(created.completed).toBe(false);

    const all = await repo.findAll();
    expect(all).toHaveLength(1);
  });

  it('should find a task by id', async () => {
    await repo.save(Task.create({ id: '2', title: 'Find me' }));
    const found = await repo.findById('2');
    expect(found).not.toBeNull();
    expect(found!.id).toBe('2');
  });

  it('should return null when task not found', async () => {
    const found = await repo.findById('nonexistent');
    expect(found).toBeNull();
  });

  it('should update a task', async () => {
    await repo.save(Task.create({ id: '3', title: 'Original' }));
    const updated = await repo.update('3', { title: 'Updated' });
    expect(updated).not.toBeNull();
    expect(updated!.title).toBe('Updated');
  });

  it('should delete a task', async () => {
    await repo.save(Task.create({ id: '4', title: 'Delete me' }));
    const deleted = await repo.delete('4');
    expect(deleted).toBe(true);
    const all = await repo.findAll();
    expect(all).toHaveLength(0);
  });
});
