import { Module } from '@nestjs/common';
import { TasksController } from './controllers/tasks.controller';
import { McpController } from './mcp/mcp.controller';
import { McpHandlerService } from './mcp/mcp.handler.service';
import { JsonTasksRepository } from './adapters/json-tasks.repository';
import { InMemoryEventBus } from './adapters/in-memory-event-bus';
import { RealtimeGateway } from './gateways/realtime.gateway';
import { TASKS_REPOSITORY_PORT } from '../application/ports/tasks-repository.port';
import { EVENT_BUS } from '../application/ports/event-bus.port';
import {
  CreateTaskUseCase,
  GetTaskUseCase,
  ListTasksUseCase,
  UpdateTaskUseCase,
  CompleteTaskUseCase,
  ReopenTaskUseCase,
  DeleteTaskUseCase,
  CreateBatchUseCase,
  ArchiveTasksUseCase,
  ListArchivedUseCase,
} from '../application/use-cases';
import { TaskEventsPublisher } from '../application/services/task-events-publisher';

@Module({
  controllers: [TasksController, McpController],
  providers: [
    {
      provide: TASKS_REPOSITORY_PORT,
      useFactory: () => new JsonTasksRepository(process.env.DATA_PATH),
    },
    {
      provide: EVENT_BUS,
      useClass: InMemoryEventBus,
    },
    TaskEventsPublisher,
    CreateTaskUseCase,
    GetTaskUseCase,
    ListTasksUseCase,
    UpdateTaskUseCase,
    CompleteTaskUseCase,
    ReopenTaskUseCase,
    DeleteTaskUseCase,
    CreateBatchUseCase,
    ArchiveTasksUseCase,
    ListArchivedUseCase,
    McpHandlerService,
    RealtimeGateway,
  ],
})
export class TasksModule {}
