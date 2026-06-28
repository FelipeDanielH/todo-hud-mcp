# todo-hud-mcp

Backend POC para Focus HUD, una app de escritorio liviana tipo todo list + pomodoro gamificado.

## Stack

- **Runtime:** Node.js ≥ 20
- **Framework:** NestJS 10 + TypeScript
- **Validación:** class-validator + class-transformer
- **Documentación:** Swagger / OpenAPI
- **Persistencia:** JSON local (reemplazable por PostgreSQL)

## Arquitectura hexagonal (puertos y adaptadores)

```
src/modules/tasks/
├── domain/                  # TypeScript puro, sin imports de framework
│   ├── models/              # Task, Priority (value object)
│   └── exceptions/          # TaskNotFoundError
├── application/             # Casos de uso y puertos abstractos
│   ├── ports/               # TasksRepositoryPort (interface + token)
│   └── use-cases/           # CreateTaskUseCase, GetTaskUseCase, ...
└── infrastructure/          # Adaptadores concretos (NestJS-aware)
    ├── adapters/            # JsonTasksRepository
    ├── controllers/         # TasksController, DTOs
    └── tasks.module.ts      # Wiring: puerto → adaptador concreto
```

### Principios

- **Domain**: cero dependencias de framework. Modelos y excepciones puras.
- **Application**: casos de uso que orquestan lógica contra puertos. Dependen solo de interfaces.
- **Infrastructure**: implementaciones concretas (JSON, HTTP, NestJS DI). El módulo mapea cada puerto a su adaptador.

## Cómo correr el servidor

```bash
npm install
npm run build
npm start
```

Servidor en `http://localhost:3000`.

## Documentación Swagger / OpenAPI

| Recurso                | URL                                |
| ---------------------- | ---------------------------------- |
| Swagger UI             | `http://localhost:3000/docs`       |
| OpenAPI JSON           | `http://localhost:3000/openapi.json` |
| OpenAPI YAML           | `http://localhost:3000/openapi.yaml` |

## Scripts

| Comando               | Descripción                       |
| --------------------- | --------------------------------- |
| `npm run build`       | Compila TypeScript → dist         |
| `npm start`           | Inicia el servidor                |
| `npm run start:dev`   | Inicia en modo watch              |
| `npm test`            | Ejecuta tests unitarios (10)      |
| `scripts/api-smoke-test.ps1` | Smoke test HTTP (requiere servidor corriendo) |

## Endpoints

| Método  | Ruta                  | Descripción             |
| ------- | --------------------- | ----------------------- |
| GET     | `/health`             | Health check            |
| GET     | `/tasks`              | Listar tareas           |
| GET     | `/tasks/:id`          | Obtener tarea por ID    |
| POST    | `/tasks`              | Crear tarea             |
| PATCH   | `/tasks/:id`          | Actualizar tarea        |
| PATCH   | `/tasks/:id/complete` | Completar tarea         |
| PATCH   | `/tasks/:id/reopen`   | Reabrir tarea           |
| DELETE  | `/tasks/:id`          | Eliminar tarea          |

## Smoke test

Con el servidor corriendo, ejecutar:

```powershell
.\scripts\api-smoke-test.ps1
```

## Migración futura

Para cambiar de JSON a PostgreSQL:

1. Implementar `TasksRepositoryPort` con Prisma/Drizzle.
2. Reemplazar `JsonTasksRepository` por la nueva clase en `tasks.module.ts`.
3. El resto del código (domain, application, controllers) no se modifica.
