# ADR-0001: Monorepo Structure

## Status

Accepted

## Context

Focus HUD started as a single NestJS backend project with REST + MCP endpoints.
As the system grows to include a desktop UI (Qt/QML), a shared type system, and
infrastructure-as-code, a single flat directory no longer scales.

We need a structure that:
- Keeps the backend and future desktop app in the same repository (shared contracts, CI/CD).
- Avoids circular dependencies and framework bleed between apps.
- Follows community conventions (Nx-inspired but without the tooling lock-in).
- Is simple enough that any contributor understands it in 30 seconds.

## Decision

Adopt a flat monorepo with three top-level directories:

| Path              | Purpose                                |
| ----------------- | -------------------------------------- |
| `apps/`           | Runnable applications (API, Desktop…)  |
| `packages/`       | Shared libraries (contracts, utils…)   |
| `infra/`          | Docker, Caddy, deploy configs          |
| `docs/`           | Project documentation and ADRs         |

Rules:
1. Each app in `apps/` has its own `package.json` and `tsconfig.json`.
2. Apps never import from sibling apps — only from `packages/`.
3. `packages/` packages are pure TypeScript / JSON Schema — no framework imports.
4. `infra/` contains only config files, no application code.
5. The root `package.json` has no build scripts — each app manages its own lifecycle.
6. Docker Compose lives at the root for discoverability; service-specific Dockerfiles
   live inside each app directory.

## Consequences

- The backend continues to work identically from `apps/api/`.
- Adding `apps/desktop/` later requires only a new directory — no structural changes.
- CI/CD workflows need minor path adjustments for build contexts.
- Developers must `cd apps/api` before running `npm install` or `npm start`.
