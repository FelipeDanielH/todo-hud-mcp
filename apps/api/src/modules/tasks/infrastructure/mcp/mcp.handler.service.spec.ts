import { McpHandlerService } from './mcp.handler.service';
import { CreateTaskUseCase } from '../../application/use-cases/create-task.use-case';
import { ListTasksUseCase } from '../../application/use-cases/list-tasks.use-case';
import { CompleteTaskUseCase } from '../../application/use-cases/complete-task.use-case';
import { CreateBatchUseCase } from '../../application/use-cases/create-batch.use-case';
import { ArchiveTasksUseCase } from '../../application/use-cases/archive-tasks.use-case';
import { TasksRepositoryPort } from '../../application/ports/tasks-repository.port';
import { Task } from '../../domain/models/task';
import { TaskNotFoundError } from '../../domain/exceptions/task-not-found.error';

describe('McpHandlerService', () => {
  let handler: McpHandlerService;
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
    handler = new McpHandlerService(
      new CreateTaskUseCase(mockRepo),
      new ListTasksUseCase(mockRepo),
      new CompleteTaskUseCase(mockRepo),
      new CreateBatchUseCase(mockRepo),
      new ArchiveTasksUseCase(mockRepo),
    );
  });

  describe('getToolList', () => {
    it('should return all three tools', () => {
      const tools = handler.getToolList();
      expect(tools).toHaveLength(5);
      const names = tools.map((t) => t.name);
      expect(names).toContain('set_tasks');
      expect(names).toContain('create_batch');
      expect(names).toContain('list_tasks');
      expect(names).toContain('complete_task');
      expect(names).toContain('archive_tasks');
    });

    it('should have correct inputSchema for complete_task', () => {
      const tool = handler
        .getToolList()
        .find((t) => t.name === 'complete_task');
      expect(tool?.inputSchema).toEqual({
        type: 'object',
        properties: {
          id: { type: 'string', description: 'ID of the task to complete' },
        },
        required: ['id'],
      });
    });
  });

  describe('complete_task', () => {
    it('should complete a task and return it', async () => {
      const task = Task.create({ id: '1', title: 'Test' });
      const completedTask = task.complete();
      mockRepo.findById.mockResolvedValue(task);
      mockRepo.update.mockResolvedValue(completedTask);

      const result = await handler.handleToolCall('complete_task', {
        id: '1',
      });

      expect(result.isError).toBeFalsy();
      expect(result.content[0].type).toBe('text');
      const parsed = JSON.parse(result.content[0].text);
      expect(parsed.completed).toBe(true);
      expect(parsed.id).toBe('1');
    });

    it('should return isError when id is missing', async () => {
      const result = await handler.handleToolCall('complete_task', {});

      expect(result.isError).toBe(true);
      expect(result.content[0].text).toBe('id must be a non-empty string');
    });

    it('should return isError when id is not a string', async () => {
      const result = await handler.handleToolCall('complete_task', {
        id: 123,
      });

      expect(result.isError).toBe(true);
      expect(result.content[0].text).toBe('id must be a non-empty string');
    });

    it('should return isError when task does not exist', async () => {
      mockRepo.findById.mockResolvedValue(null);

      const result = await handler.handleToolCall('complete_task', {
        id: 'nonexistent',
      });

      expect(result.isError).toBe(true);
      expect(result.content[0].text).toContain('Task not found');
    });
  });

  describe('set_tasks', () => {
    it('should still work after adding complete_task', async () => {
      const task = Task.create({ id: '1', title: 'New' });
      mockRepo.save.mockResolvedValue(task);

      const result = await handler.handleToolCall('set_tasks', {
        titles: ['New'],
      });

      expect(result.isError).toBeFalsy();
      const parsed = JSON.parse(result.content[0].text);
      expect(parsed.count).toBe(1);
    });
  });

  describe('list_tasks', () => {
    it('should still work after adding complete_task', async () => {
      mockRepo.findAll.mockResolvedValue([]);

      const result = await handler.handleToolCall('list_tasks', {});

      expect(result.isError).toBeFalsy();
      const parsed = JSON.parse(result.content[0].text);
      expect(parsed.count).toBe(0);
    });
  });

  describe('unknown tool', () => {
    it('should return isError for unknown tool', async () => {
      const result = await handler.handleToolCall('bogus', {});

      expect(result.isError).toBe(true);
      expect(result.content[0].text).toContain('Unknown tool');
    });
  });
});
