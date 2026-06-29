import {
  Controller,
  Get,
  Post,
  Patch,
  Delete,
  Param,
  Body,
  NotFoundException,
  Query,
} from '@nestjs/common';
import {
  ApiTags,
  ApiOperation,
  ApiOkResponse,
  ApiCreatedResponse,
  ApiBadRequestResponse,
  ApiNotFoundResponse,
  ApiQuery,
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
  CreateBatchUseCase,
  ArchiveTasksUseCase,
  ListArchivedUseCase,
} from '../../application/use-cases';
import { CreateTaskDto, UpdateTaskDto, CreateBatchDto, ArchiveBodyDto } from './dto';
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
    private readonly createBatchUseCase: CreateBatchUseCase,
    private readonly archiveTasksUseCase: ArchiveTasksUseCase,
    private readonly listArchivedUseCase: ListArchivedUseCase,
  ) {}

  @Get()
  @ApiOperation({ summary: 'List all tasks' })
  @ApiOkResponse({ description: 'List of tasks' })
  @ApiQuery({ name: 'status', required: false, description: 'Filter by status (pending, completed, archived)' })
  async findAll(@Query('status') status?: string): Promise<Task[]> {
    const all = await this.listTasks.execute();
    if (status) {
      return all.filter((t) => t.status === status);
    }
    return all;
  }

  @Get('history')
  @ApiOperation({ summary: 'List archived tasks grouped by phase' })
  @ApiOkResponse({ description: 'Archived phases with tasks' })
  async history(): Promise<unknown> {
    return this.listArchivedUseCase.execute();
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

  @Post('batch')
  @ApiOperation({ summary: 'Create a batch of tasks under a phase' })
  @ApiCreatedResponse({ description: 'Batch created with phaseId and tasks' })
  @ApiBadRequestResponse({ description: 'Invalid input' })
  async createBatch(@Body() dto: CreateBatchDto): Promise<{
    phaseId: string;
    phaseName: string;
    tasks: Task[];
  }> {
    return this.createBatchUseCase.execute(dto);
  }

  @Post('archive')
  @ApiOperation({ summary: 'Archive all completed tasks, optionally filtered by phaseId' })
  @ApiOkResponse({ description: 'Tasks archived' })
  async archive(@Body() dto: ArchiveBodyDto): Promise<{ archived: number; archivedAt: string }> {
    return this.archiveTasksUseCase.execute(dto.phaseId);
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
