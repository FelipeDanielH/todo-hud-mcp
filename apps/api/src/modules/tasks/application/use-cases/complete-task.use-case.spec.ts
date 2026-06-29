import { CompleteTaskUseCase } from './complete-task.use-case';
import { TasksRepositoryPort } from '../ports/tasks-repository.port';
import { Task } from '../../domain/models/task';
import { TaskNotFoundError } from '../../domain/exceptions';

describe('CompleteTaskUseCase', () => {
  let useCase: CompleteTaskUseCase;
  let mockRepo: jest.Mocked<TasksRepositoryPort>;

  beforeEach(() => {
    mockRepo = {
      findAll: jest.fn(),
      findById: jest.fn(),
      save: jest.fn(),
      update: jest.fn(),
      delete: jest.fn(),
    };
    useCase = new CompleteTaskUseCase(mockRepo);
  });

  it('should complete a task', async () => {
    const task = Task.create({ id: '1', title: 'Test' });
    mockRepo.findById.mockResolvedValue(task);
    mockRepo.update.mockImplementation(async (_id, data) => {
      const now = new Date().toISOString();
      return new Task(
        task.id,
        data.title ?? task.title,
        data.description ?? task.description,
        true,
        data.priority ?? task.priority,
        task.createdAt,
        now,
        now,
      );
    });

    const result = await useCase.execute('1');
    expect(result.completed).toBe(true);
    expect(result.completedAt).toBeDefined();
  });

  it('should throw when task not found', async () => {
    mockRepo.findById.mockResolvedValue(null);
    await expect(useCase.execute('bad')).rejects.toThrow(TaskNotFoundError);
  });
});
