# todo-hud-mcp

Backend POC para Focus HUD, una app de escritorio liviana tipo todo list + pomodoro gamificado.

## Stack

- **Runtime:** Node.js ≥ 20
- **Framework:** NestJS 11 + TypeScript
- **Validación:** class-validator + class-transformer + zod
- **Documentación:** Swagger / OpenAPI
- **MCP:** Model Context Protocol (tools `set_tasks`, `list_tasks`)
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
    ├── mcp/                 # Servidor MCP (tsx, proceso separado ESM)
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
| `npm run mcp`         | Inicia servidor MCP (stdio)        |
| `npm test`            | Ejecuta tests unitarios (10)      |
| `scripts/api-smoke-test.ps1` | Smoke test REST (requiere servidor corriendo) |
| `scripts/mcp-smoke-test.ps1` | Smoke test MCP HTTP (requiere servidor corriendo) |

## Endpoints

| Método  | Ruta                  | Descripción                      |
| ------- | --------------------- | -------------------------------- |
| GET     | `/health`             | Health check                     |
| GET     | `/tasks`              | Listar tareas                    |
| GET     | `/tasks/:id`          | Obtener tarea por ID             |
| POST    | `/tasks`              | Crear tarea                      |
| PATCH   | `/tasks/:id`          | Actualizar tarea                 |
| PATCH   | `/tasks/:id/complete` | Completar tarea                  |
| PATCH   | `/tasks/:id/reopen`   | Reabrir tarea                    |
| DELETE  | `/tasks/:id`          | Eliminar tarea                   |
| POST    | `/mcp`                | MCP Streamable HTTP (JSON-RPC 2.0) |

## Smoke test

Con el servidor corriendo, ejecutar:

```powershell
.\scripts\api-smoke-test.ps1
```

## MCP (Model Context Protocol)

El backend expone MCP en dos modalidades: **stdio** (proceso separado ESM) y **HTTP** (endpoint REST dentro de NestJS). Ambos usan las mismas tools (`set_tasks`, `list_tasks`) a través de `McpHandlerService`, sin duplicación de lógica.

### Modalidad stdio (clientes locales)

```bash
npm run mcp
```

Corre como proceso independiente (ESM puro vía `tsx`) sobre stdio. Ideal para integrar con Claude Desktop, Cursor, o cualquier cliente MCP local.

### Modalidad HTTP (clientes remotos)

El endpoint `POST /mcp` acepta mensajes JSON-RPC 2.0 según el protocolo MCP Streamable HTTP.

| Herramienta   | Input                          | Output          |
| ------------- | ------------------------------ | --------------- |
| `set_tasks`   | `{ titles: string[] }`         | Tasks creados   |
| `list_tasks`  | `{}`                           | Todos los tasks |

**Handshake** (`initialize`):

```bash
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"my-client","version":"1.0.0"}}}'
```

**Listar herramientas** (`tools/list`):

```bash
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}'
```

**Ejecutar herramienta** (`tools/call`):

```bash
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"set_tasks","arguments":{"titles":["Comprar leche","Estudiar NestJS"]}}}'
```

**Notificaciones** (sin `id`): responden con `202 Accepted` sin body.

### Probar con MCP Inspector

[MCP Inspector](https://github.com/modelcontextprotocol/inspector) permite probar ambos modos visualmente.

**Modo stdio:**

```bash
npx @modelcontextprotocol/inspector tsx src/modules/tasks/infrastructure/mcp/tasks-mcp-server.mts
```

**Modo HTTP (Streamable HTTP):**

El Inspector no soporta nativamente Streamable HTTP vía URL. Para probar, usar `curl` o el script `scripts/mcp-smoke-test.ps1`.

### Despliegue con HTTPS (VPS)

Para conectar desde ChatGPT Apps/Connectors o cualquier cliente MCP remoto:

1. **Proxy reverso** con Caddy o Nginx + Let's Encrypt:

```nginx
# /etc/nginx/sites-available/focus-hud
server {
    listen 443 ssl;
    server_name focus-hud.tudominio.com;

    ssl_certificate /etc/letsencrypt/live/focus-hud.tudominio.com/fullchain.pem;
    ssl_certificate_key /etc/letsencrypt/live/focus-hud.tudominio.com/privkey.pem;

    location / {
        proxy_pass http://127.0.0.1:3000;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection 'upgrade';
        proxy_set_header Host $host;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
        proxy_cache_bypass $http_upgrade;
    }
}
```

2. **Servir con PM2** para producción:

```bash
npm run build
pm2 start dist/main.js --name focus-hud
pm2 save
```

3. **Conectar desde ChatGPT**:
   - Crear un GPT Action con la OpenAPI spec en `https://focus-hud.tudominio.com/openapi.json`
   - O usar un cliente MCP que soporte Streamable HTTP apuntando a `https://focus-hud.tudominio.com/mcp`

### Arquitectura MCP

```
src/modules/tasks/infrastructure/mcp/
├── mcp.handler.service.ts    # Servicio compartido (usado por stdio y HTTP)
├── mcp.controller.ts         # NestJS controller para POST /mcp
└── tasks-mcp-server.mts      # Servidor stdio (ESM, vía tsx)
```

- `McpHandlerService` es un `@Injectable()` de NestJS que wrappe los use cases y expone `handleToolCall(name, args)`.
- El controller HTTP (`POST /mcp`) implementa JSON-RPC 2.0 + Streamable HTTP (SSE opcional con `Accept: text/event-stream`).
- El servidor stdio instancia `McpHandlerService` manualmente (sin NestJS DI) y registra las tools en `McpServer` con esquemas Zod.

## Migración futura

Para cambiar de JSON a PostgreSQL:

1. Implementar `TasksRepositoryPort` con Prisma/Drizzle.
2. Reemplazar `JsonTasksRepository` por la nueva clase en `tasks.module.ts`.
3. El resto del código (domain, application, controllers) no se modifica.
