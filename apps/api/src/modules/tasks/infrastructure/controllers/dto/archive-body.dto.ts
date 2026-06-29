import { IsOptional, IsString } from 'class-validator';
import { ApiPropertyOptional } from '@nestjs/swagger';

export class ArchiveBodyDto {
  @ApiPropertyOptional({ description: 'If provided, only archive completed tasks in this phase' })
  @IsOptional()
  @IsString()
  phaseId?: string;
}
