# Deploy Focus HUD en VPS (producción)

Guía para desplegar el backend Focus HUD como servidor MCP HTTP en un VPS con Docker.

## Requisitos

- VPS con Ubuntu 22.04+
- Docker y Docker Compose instalados
- Dominio (opcional, para HTTPS con Let's Encrypt)

## 1. Instalar Docker

```bash
curl -fsSL https://get.docker.com | sh
sudo usermod -aG docker $USER
# Cerrar sesión y volver a entrar para aplicar el grupo
```

Instalar Docker Compose plugin:

```bash
sudo apt update
sudo apt install docker-compose-plugin
```

## 2. Clonar el repositorio

```bash
git clone https://github.com/FelipeDanielH/todo-hud-mcp.git
cd todo-hud-mcp
```

## 3. Configurar entorno

```bash
cp .env.example .env
# Editar .env si es necesario (valores por defecto funcionan)
```

## 4. Construir y levantar

```bash
docker compose up -d --build
```

Esto construye la imagen, crea el contenedor y lo inicia en background.

## 5. Verificar health check

```bash
curl http://localhost:3000/health
```

Respuesta esperada:

```json
{
  "status": "ok",
  "timestamp": "2026-06-28T...",
  "uptime": 12.34
}
```

## 6. Probar endpoint MCP

```bash
# Initialize
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-03-26","capabilities":{},"clientInfo":{"name":"vps-test","version":"1.0.0"}}}'

# tools/list
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}'

# tools/call — set_tasks
curl -X POST http://localhost:3000/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"set_tasks","arguments":{"titles":["Tarea 1","Tarea 2"]}}}'
```

## 7. Ver logs

```bash
docker compose logs -f
```

## 8. Detener

```bash
docker compose down
```

Para detener y eliminar el volumen de datos:

```bash
docker compose down -v
```

## 9. HTTPS con Nginx + Let's Encrypt (opcional pero recomendado)

### 9.1. Instalar Nginx

```bash
sudo apt install nginx
```

### 9.2. Configurar dominio y SSL

```bash
sudo apt install certbot python3-certbot-nginx
sudo certbot --nginx -d focus-hud.tudominio.com
```

### 9.3. Configurar Nginx como proxy reverso

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

    # Redirigir HTTP → HTTPS
    server_name focus-hud.tudominio.com;
    listen 80;
    return 301 https://$server_name$request_uri;
}
```

```bash
sudo ln -s /etc/nginx/sites-available/focus-hud /etc/nginx/sites-enabled/
sudo nginx -t
sudo systemctl reload nginx
```

### 9.4. Conectar desde ChatGPT

Una vez con HTTPS funcionando:

1. Abrir ChatGPT → Explorar → Crear GPT → Configurar → Actions
2. Importar desde URL: `https://focus-hud.tudominio.com/openapi.json`
3. Las acciones disponibles incluirán los endpoints REST y MCP.

O usando un cliente MCP que soporte Streamable HTTP, apuntar a:

```
https://focus-hud.tudominio.com/mcp
```

## 10. Comandos útiles

| Comando                                      | Descripción                         |
| -------------------------------------------- | ----------------------------------- |
| `docker compose up -d --build`               | Construir y levantar                |
| `docker compose down`                        | Detener                             |
| `docker compose logs -f`                     | Logs en tiempo real                 |
| `docker compose exec focus-hud wget -qO- http://localhost:3000/health` | Health check desde adentro |
| `docker volume ls`                           | Listar volúmenes                    |
| `docker compose ps`                          | Estado del contenedor               |

## Estructura de archivos para producción

```
todo-hud-mcp/
├── Dockerfile              # Multi-stage build
├── .dockerignore           # Excluye node_modules, dist, etc.
├── docker-compose.yml      # Orquestación con healthcheck + volumen
├── .env                    # Variables de entorno (no versionado)
├── .env.example            # Template de configuración
├── docs/
│   └── deploy-vps.md       # Esta guía
└── src/…                   # Código fuente (compilado en builder stage)
```

El volumen `focus-hud-data` persiste `tasks.json` en `/data` dentro del contenedor.
