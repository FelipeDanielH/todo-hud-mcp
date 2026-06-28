import { Module } from '@nestjs/common';
import { TasksController } from './controllers/tasks.controller';
import { McpController } from './mcp/mcp.controller';
import { McpHandlerService } from './mcp/mcp.handler.service';
import { JsonTasksRepository } from './adapters/json-tasks.repository';
import { TASKS_REPOSITORY_PORT } from '../application/ports/tasks-repository.port';
import {
  CreateTaskUseCase,
  GetTaskUseCase,
  ListTasksUseCase,
  UpdateTaskUseCase,
  CompleteTaskUseCase,
  ReopenTaskUseCase,
  DeleteTaskUseCase,
} from '../application/use-cases';

@Module({
  controllers: [TasksController, McpController],
  providers: [
    { provide: TASKS_REPOSITORY_PORT, useClass: JsonTasksRepository },
    CreateTaskUseCase,
    GetTaskUseCase,
    ListTasksUseCase,
    UpdateTaskUseCase,
    CompleteTaskUseCase,
    ReopenTaskUseCase,
    DeleteTaskUseCase,
    McpHandlerService,
  ],
})
export class TasksModule {}
