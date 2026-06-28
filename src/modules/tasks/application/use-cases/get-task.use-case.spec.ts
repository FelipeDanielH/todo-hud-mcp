import { GetTaskUseCase } from './get-task.use-case';
import { TasksRepositoryPort } from '../ports/tasks-repository.port';
import { Task } from '../../domain/models/task';
import { TaskNotFoundError } from '../../domain/exceptions';

describe('GetTaskUseCase', () => {
  let useCase: GetTaskUseCase;
  let mockRepo: jest.Mocked<TasksRepositoryPort>;

  beforeEach(() => {
    mockRepo = {
      findAll: jest.fn(),
      findById: jest.fn(),
      save: jest.fn(),
      update: jest.fn(),
      delete: jest.fn(),
    };
    useCase = new GetTaskUseCase(mockRepo);
  });

  it('should return a task when found', async () => {
    const task = Task.create({ id: '1', title: 'Test' });
    mockRepo.findById.mockResolvedValue(task);

    const result = await useCase.execute('1');
    expect(result.id).toBe('1');
  });

  it('should throw when not found', async () => {
    mockRepo.findById.mockResolvedValue(null);
    await expect(useCase.execute('bad')).rejects.toThrow(TaskNotFoundError);
  });
});
