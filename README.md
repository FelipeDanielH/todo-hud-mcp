# Focus HUD Monorepo

Backend POC + desktop UI for Focus HUD, a lightweight todo + gamified pomodoro app.

## Structure

```
apps/api/               # NestJS backend (REST + MCP)
packages/shared/        # Shared contracts/types (future)
infra/docker/           # Docker Compose config
infra/caddy/            # Caddy reverse proxy config (future)
docs/adr/               # Architecture Decision Records
```

## Quick start (backend)

```bash
cd apps/api
npm install
npm run build
npm start
```

Server at `http://localhost:3000`. Swagger at `/docs`.

## Docker

```bash
docker compose up -d --build
```

## MCP

- **HTTP:** `POST /mcp` (inside NestJS)
- **Stdio:** `cd apps/api && npm run mcp`

## Docs

- [Deploy guide](docs/deploy-vps.md)
- [ADR-0001: Monorepo structure](docs/adr/0001-monorepo-structure.md)
