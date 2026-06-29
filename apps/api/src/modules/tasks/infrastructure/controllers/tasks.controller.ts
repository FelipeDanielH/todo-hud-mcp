import {
  Controller,
  Get,
  Post,
  Patch,
  Delete,
  Param,
  Body,
  NotFoundException,
} from '@nestjs/common';
import {
  ApiTags,
  ApiOperation,
  ApiOkResponse,
  ApiCreatedResponse,
  ApiBadRequestResponse,
  ApiNotFoundResponse,
} from '@nestjs/swagger';
import { TaskNotFoundError } from '../../domain/exceptions';
import {
  CreateTaskUseCase,
  GetTaskUseCase,
  ListTasksUseCase,
  UpdateTaskUseCase,
  CompleteTaskUseCase,
  ReopenTaskUseCase,
  DeleteTaskUseCase,
} from '../../application/use-cases';
import { CreateTaskDto, UpdateTaskDto } from './dto';
import { Task } from '../../domain/models';

@ApiTags('Tasks')
@Controller('tasks')
export class TasksController {
  constructor(
    private readonly createTask: CreateTaskUseCase,
    private readonly getTask: GetTaskUseCase,
    private readonly listTasks: ListTasksUseCase,
    private readonly updateTask: UpdateTaskUseCase,
    private readonly completeTask: CompleteTaskUseCase,
    private readonly reopenTask: ReopenTaskUseCase,
    private readonly deleteTask: DeleteTaskUseCase,
  ) {}

  @Get()
  @ApiOperation({ summary: 'List all tasks' })
  @ApiOkResponse({ description: 'List of tasks' })
  async findAll(): Promise<Task[]> {
    return this.listTasks.execute();
  }

  @Get(':id')
  @ApiOperation({ summary: 'Get a task by ID' })
  @ApiOkResponse({ description: 'The task' })
  @ApiNotFoundResponse({ description: 'Task not found' })
  async findById(@Param('id') id: string): Promise<Task> {
    try {
      return await this.getTask.execute(id);
    } catch (error) {
      if (error instanceof TaskNotFoundError) throw new NotFoundException(error.message);
      throw error;
    }
  }

  @Post()
  @ApiOperation({ summary: 'Create a new task' })
  @ApiCreatedResponse({ description: 'Task created' })
  @ApiBadRequestResponse({ description: 'Invalid input' })
  async create(@Body() dto: CreateTaskDto): Promise<Task> {
    return this.createTask.execute(dto);
  }

  @Patch(':id')
  @ApiOperation({ summary: 'Update a task' })
  @ApiOkResponse({ description: 'Task updated' })
  @ApiNotFoundResponse({ description: 'Task not found' })
  @ApiBadRequestResponse({ description: 'Invalid input' })
  async update(
    @Param('id') id: string,
    @Body() dto: UpdateTaskDto,
  ): Promise<Task> {
    try {
      return await this.updateTask.execute(id, dto);
    } catch (error) {
      if (error instanceof TaskNotFoundError) throw new NotFoundException(error.message);
      throw error;
    }
  }

  @Patch(':id/complete')
  @ApiOperation({ summary: 'Mark a task as completed' })
  @ApiOkResponse({ description: 'Task completed' })
  @ApiNotFoundResponse({ description: 'Task not found' })
  async complete(@Param('id') id: string): Promise<Task> {
    try {
      return await this.completeTask.execute(id);
    } catch (error) {
      if (error instanceof TaskNotFoundError) throw new NotFoundException(error.message);
      throw error;
    }
  }

  @Patch(':id/reopen')
  @ApiOperation({ summary: 'Reopen a completed task' })
  @ApiOkResponse({ description: 'Task reopened' })
  @ApiNotFoundResponse({ description: 'Task not found' })
  async reopen(@Param('id') id: string): Promise<Task> {
    try {
      return await this.reopenTask.execute(id);
    } catch (error) {
      if (error instanceof TaskNotFoundError) throw new NotFoundException(error.message);
      throw error;
    }
  }

  @Delete(':id')
  @ApiOperation({ summary: 'Delete a task' })
  @ApiOkResponse({ description: 'Task deleted' })
  @ApiNotFoundResponse({ description: 'Task not found' })
  async delete(@Param('id') id: string): Promise<void> {
    try {
      await this.deleteTask.execute(id);
    } catch (error) {
      if (error instanceof TaskNotFoundError) throw new NotFoundException(error.message);
      throw error;
    }
  }
}
