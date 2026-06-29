param(
  [string]$BaseUrl = "http://localhost:3000"
)

$ErrorActionPreference = "Stop"

function Test-Endpoint {
  param([string]$Method, [string]$Path, [object]$Body, [int]$ExpectedStatus)
  $url = "$BaseUrl$Path"
  $params = @{ Uri = $url; Method = $Method }
  if ($Body) {
    $params.Body = ($Body | ConvertTo-Json -Depth 10 -Compress)
    $params.ContentType = "application/json"
  }
  try {
    $res = Invoke-RestMethod @params -StatusCodeVariable statusCode -SkipHttpErrorCheck
    if ($statusCode -ne $ExpectedStatus) {
      Write-Host "FAIL [$Method $Path] expected $ExpectedStatus got $statusCode" -ForegroundColor Red
      exit 1
    }
    Write-Host "PASS [$Method $Path] → $statusCode" -ForegroundColor Green
    return $res
  } catch {
    Write-Host "FAIL [$Method $Path] $($_.Exception.Message)" -ForegroundColor Red
    exit 1
  }
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "Focus HUD API — Smoke Test" -ForegroundColor Cyan
Write-Host "Base URL: $BaseUrl" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# 1. Health
Write-Host "`n--- Health ---" -ForegroundColor Yellow
$health = Test-Endpoint -Method GET -Path "/health" -ExpectedStatus 200
if ($health.status -ne "ok") { throw "Health check failed" }

# 2. Create task
Write-Host "`n--- Create Task ---" -ForegroundColor Yellow
$task = Test-Endpoint -Method POST -Path "/tasks" -Body @{
  title       = "Smoke test task"
  description = "Created by smoke test"
  priority    = "high"
} -ExpectedStatus 201
$taskId = $task.id

# 3. List tasks
Write-Host "`n--- List Tasks ---" -ForegroundColor Yellow
$tasks = Test-Endpoint -Method GET -Path "/tasks" -ExpectedStatus 200
if ($tasks.Count -lt 1) { throw "Expected at least 1 task" }

# 4. Get task by id
Write-Host "`n--- Get Task ---" -ForegroundColor Yellow
$fetched = Test-Endpoint -Method GET -Path "/tasks/$taskId" -ExpectedStatus 200
if ($fetched.id -ne $taskId) { throw "Task ID mismatch" }

# 5. Update task
Write-Host "`n--- Update Task ---" -ForegroundColor Yellow
$updated = Test-Endpoint -Method PATCH -Path "/tasks/$taskId" -Body @{
  title = "Updated title"
  description = "Updated description"
} -ExpectedStatus 200
if ($updated.title -ne "Updated title") { throw "Title not updated" }

# 6. Complete task
Write-Host "`n--- Complete Task ---" -ForegroundColor Yellow
$completed = Test-Endpoint -Method PATCH -Path "/tasks/$taskId/complete" -ExpectedStatus 200
if ($completed.completed -ne $true) { throw "Task not marked as completed" }

# 7. Reopen task
Write-Host "`n--- Reopen Task ---" -ForegroundColor Yellow
$reopened = Test-Endpoint -Method PATCH -Path "/tasks/$taskId/reopen" -ExpectedStatus 200
if ($reopened.completed -ne $false) { throw "Task not reopened" }

# 8. Delete task
Write-Host "`n--- Delete Task ---" -ForegroundColor Yellow
Test-Endpoint -Method DELETE -Path "/tasks/$taskId" -ExpectedStatus 200

# 9. Invalid create task
Write-Host "`n--- Invalid Create (bad request) ---" -ForegroundColor Yellow
Test-Endpoint -Method POST -Path "/tasks" -Body @{ title = "" } -ExpectedStatus 400

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "All smoke tests passed!" -ForegroundColor Green
Write-Host "========================================`n" -ForegroundColor Cyan
