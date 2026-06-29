export const mockTaskEventsPublisher = {
  taskCreated: jest.fn().mockResolvedValue(undefined),
  taskBatchCreated: jest.fn().mockResolvedValue(undefined),
  taskCompleted: jest.fn().mockResolvedValue(undefined),
  taskReopened: jest.fn().mockResolvedValue(undefined),
  tasksArchived: jest.fn().mockResolvedValue(undefined),
};
