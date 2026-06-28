import { NestFactory } from '@nestjs/core';
import { ValidationPipe } from '@nestjs/common';
import { NestExpressApplication } from '@nestjs/platform-express';
import { SwaggerModule, DocumentBuilder } from '@nestjs/swagger';
import * as yaml from 'js-yaml';
import { AppModule } from './app.module';
import { AllExceptionsFilter } from './common/filters/http-exception.filter';

async function bootstrap(): Promise<void> {
  const app = await NestFactory.create<NestExpressApplication>(AppModule);
  app.set('query parser', 'extended');

  app.useGlobalPipes(
    new ValidationPipe({
      whitelist: true,
      forbidNonWhitelisted: true,
      transform: true,
    }),
  );

  app.useGlobalFilters(new AllExceptionsFilter());

  const swaggerConfig = new DocumentBuilder()
    .setTitle('Focus HUD API')
    .setDescription('Backend API for Focus HUD — todo list + pomodoro gamificado')
    .setVersion('0.1.0')
    .build();

  const document = SwaggerModule.createDocument(app, swaggerConfig);
  SwaggerModule.setup('docs', app, document);

  const httpAdapter = app.getHttpAdapter();
  httpAdapter.get('/openapi.json', (_req: any, res: any) => {
    res.json(document);
  });
  httpAdapter.get('/openapi.yaml', (_req: any, res: any) => {
    res.type('text/yaml').send(yaml.dump(document));
  });

  const port = process.env.PORT ?? 3000;
  await app.listen(port);
  console.log(`Focus HUD backend running on http://localhost:${port}`);
  console.log(`Swagger UI: http://localhost:${port}/docs`);
  console.log(`OpenAPI JSON: http://localhost:${port}/openapi.json`);
  console.log(`OpenAPI YAML: http://localhost:${port}/openapi.yaml`);
}

bootstrap();
