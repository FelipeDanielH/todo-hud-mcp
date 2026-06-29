# Migración a PostgreSQL con NeonDB

## Cuándo migrar

- Cuando la carga requiera persistencia concurrente (múltiples usuarios/instancias)
- Cuando se necesiten consultas avanzadas (filtros, joins, agregaciones)
- Cuando se quiera desplegar en producción con alta disponibilidad

## Paquetes necesarios

```bash
npm install @neondatabase/serverless
# o con Prisma
npm install @prisma/client
npm install -D prisma
npx prisma init
```

## Pasos para migrar

### 1. Crear el esquema SQL

```sql
CREATE TABLE tasks (
  id UUID PRIMARY KEY,
  title VARCHAR(200) NOT NULL,
  description TEXT,
  completed BOOLEAN NOT NULL DEFAULT FALSE,
  priority VARCHAR(10) CHECK (priority IN ('low', 'medium', 'high')),
  created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
  completed_at TIMESTAMPTZ
);
```

### 2. Implementar NeonTasksRepository

Editar `src/modules/tasks/infrastructure/adapters/neon/neon-tasks.repository.ts`
y completar los 5 métodos usando `@neondatabase/serverless`.

### 3. Cambiar el provider en TasksModule

```typescript
// tasks.module.ts
{
  provide: TASKS_REPOSITORY_PORT,
  useFactory: () => new NeonTasksRepository(process.env.DATABASE_URL!),
}
```

### 4. Configurar variable de entorno

```env
DATABASE_URL=postgresql://user:pass@ep-xxx.us-east-2.aws.neon.tech/neondb?sslmode=require
```

### 5. Verificar

```bash
npm run build && npm test
```

Los casos de uso y controladores no requieren cambios — dependen solo de `TasksRepositoryPort`.
