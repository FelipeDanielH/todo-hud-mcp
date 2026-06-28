import { Module } from '@nestjs/common';
import { HealthModule } from './health/health.module';
import { TasksModule } from './modules/tasks/infrastructure/tasks.module';

@Module({
  imports: [HealthModule, TasksModule],
})
export class AppModule {}
