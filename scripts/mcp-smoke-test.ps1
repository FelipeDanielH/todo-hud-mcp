#!/usr/bin/env pwsh
<#
.SYNOPSIS
  Smoke test para el endpoint MCP HTTP (POST /mcp).
  Requiere el servidor corriendo (npm start).
#>

$BASE = "http://localhost:3000"
$PASS = 0
$FAIL = 0

function Test-McpRequest {
  param([string]$Name, [object]$Body, [scriptblock]$Assert)
  try {
    $json = $Body | ConvertTo-Json -Compress
    $response = Invoke-RestMethod -Uri "$BASE/mcp" -Method Post -Body $json -ContentType "application/json"
    $ok = & $Assert $response
    if ($ok) { Write-Host "PASS $Name" -ForegroundColor Green; $script:PASS++ }
    else { Write-Host "FAIL $Name (unexpected response)" -ForegroundColor Red; $script:FAIL++ }
  } catch {
    Write-Host "FAIL $Name ($($_.Exception.Message))" -ForegroundColor Red
    $script:FAIL++
  }
}

Write-Host "=== MCP HTTP Smoke Test ===" -ForegroundColor Cyan

# 1. Initialize
Test-McpRequest -Name "initialize" -Body @{
  jsonrpc = "2.0"; id = 1; method = "initialize"
  params = @{ protocolVersion = "2025-03-26"; capabilities = @{}; clientInfo = @{ name = "smoke-test"; version = "1.0.0" } }
} -Assert { $_.result.serverInfo.name -eq "focus-hud" }

# 2. Tools list
Test-McpRequest -Name "tools/list" -Body @{
  jsonrpc = "2.0"; id = 2; method = "tools/list"; params = @{}
} -Assert { $_.result.tools.Count -ge 2 }

# 3. set_tasks
Test-McpRequest -Name "set_tasks" -Body @{
  jsonrpc = "2.0"; id = 3; method = "tools/call"
  params = @{ name = "set_tasks"; arguments = @{ titles = @("MCP smoke A", "MCP smoke B") } }
} -Assert { $_.result.content[0].text -match '"count": 2' }

# 4. list_tasks
Test-McpRequest -Name "list_tasks" -Body @{
  jsonrpc = "2.0"; id = 4; method = "tools/call"
  params = @{ name = "list_tasks"; arguments = @{} }
} -Assert { $_.result.content[0].text -match '"count"' }

# 5. Notification (no id) - espera 202 Accepted
try {
  $notifBody = @{ jsonrpc = "2.0"; method = "notifications/initialized" } | ConvertTo-Json -Compress
  $notifResp = Invoke-WebRequest -Uri "$BASE/mcp" -Method Post -Body $notifBody -ContentType "application/json"
  if ($notifResp.StatusCode -eq 202) { Write-Host "PASS notification (202)" -ForegroundColor Green; $PASS++ }
  else { Write-Host "FAIL notification (status $($notifResp.StatusCode))" -ForegroundColor Red; $FAIL++ }
} catch {
  Write-Host "FAIL notification ($($_.Exception.Message))" -ForegroundColor Red; $FAIL++
}

# 6. Unknown method
Test-McpRequest -Name "unknown_method" -Body @{
  jsonrpc = "2.0"; id = 5; method = "bogus"; params = @{}
} -Assert { $_.error.code -eq -32601 }

Write-Host "=== Results: $PASS passed, $FAIL failed ===" -ForegroundColor Cyan
exit $FAIL
