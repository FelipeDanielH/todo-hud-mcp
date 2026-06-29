import { IsString, IsArray, ArrayNotEmpty, MinLength, MaxLength } from 'class-validator';
import { ApiProperty } from '@nestjs/swagger';

export class CreateBatchDto {
  @ApiProperty({ description: 'Phase name for the batch', example: 'Lab 281 - Fase 1', minLength: 1 })
  @IsString()
  @MinLength(1)
  phaseName!: string;

  @ApiProperty({ description: 'List of task titles', example: ['Abrir consola AWS', 'Crear alarma'], minItems: 1 })
  @IsArray()
  @ArrayNotEmpty()
  @IsString({ each: true })
  @MinLength(1, { each: true })
  @MaxLength(200, { each: true })
  tasks!: string[];
}
