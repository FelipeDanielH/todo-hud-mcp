import { CreateTaskUseCase } from './create-task.use-case';
import { TasksRepositoryPort } from '../ports/tasks-repository.port';
import { Task } from '../../domain/models/task';
import { mockTaskEventsPublisher } from '../services/task-events-publisher.mock';

jest.mock('../services/task-events-publisher');

describe('CreateTaskUseCase', () => {
  let useCase: CreateTaskUseCase;
  let mockRepo: jest.Mocked<TasksRepositoryPort>;

  beforeEach(() => {
    mockRepo = {
      findAll: jest.fn(),
      findById: jest.fn(),
      findByPhase: jest.fn(),
      findArchived: jest.fn(),
      save: jest.fn(),
      saveAll: jest.fn(),
      update: jest.fn(),
      delete: jest.fn(),
    };
    useCase = new CreateTaskUseCase(mockRepo, mockTaskEventsPublisher as any);
  });

  it('should create a task', async () => {
    mockRepo.save.mockImplementation(async (t) => t);

    const result = await useCase.execute({ title: 'Hello' });

    expect(result.title).toBe('Hello');
    expect(result.completed).toBe(false);
    expect(result.id).toBeDefined();
    expect(mockRepo.save).toHaveBeenCalledTimes(1);
  });
});
