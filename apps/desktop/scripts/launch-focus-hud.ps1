param()

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$desktopDir = Resolve-Path -LiteralPath (Join-Path $scriptDir "..")
$buildDir = Join-Path $desktopDir "build"
$exe = Join-Path $buildDir "FocusHud.exe"
$logDir = Join-Path $env:LOCALAPPDATA "FocusHUD"
$logFile = Join-Path $logDir "launch.log"
$qtMingwBin = "C:\Qt\Tools\mingw1310_64\bin"
$qtRuntimeBin = "C:\Qt\6.11.1\mingw_64\bin"

function Show-LaunchError {
    param([string]$Message)

    try {
        $shell = New-Object -ComObject WScript.Shell
        $null = $shell.Popup("$Message`n`nLog: $logFile", 0, "Focus HUD", 16)
    } catch {
    }
}

function Invoke-LoggedCommand {
    param(
        [string]$FilePath,
        [string[]]$Arguments
    )

    $quoted = $Arguments | ForEach-Object {
        if ($_ -match "\s") { '"' + $_ + '"' } else { $_ }
    }
    Add-Content -LiteralPath $logFile -Value "> $FilePath $($quoted -join ' ')"
    & $FilePath @Arguments *>> $logFile
    if ($LASTEXITCODE -ne 0) {
        throw "$FilePath failed with exit code $LASTEXITCODE"
    }
}

try {
    New-Item -ItemType Directory -Force -Path $logDir | Out-Null
    Set-Content -LiteralPath $logFile -Value "Focus HUD launcher $(Get-Date -Format s)"

    $running = Get-Process FocusHud -ErrorAction SilentlyContinue | Where-Object {
        $_.Path -eq $exe
    }
    if ($running) {
        Add-Content -LiteralPath $logFile -Value "Focus HUD is already running; skipping build."
        exit 0
    }

    foreach ($path in @($qtMingwBin, $qtRuntimeBin)) {
        if (-not (Test-Path -LiteralPath $path)) {
            throw "Required Qt path not found: $path"
        }
    }

    $env:PATH = "$qtMingwBin;$qtRuntimeBin;$env:PATH"

    Push-Location $desktopDir
    try {
        if (-not (Test-Path -LiteralPath (Join-Path $buildDir "CMakeCache.txt"))) {
            if (Test-Path -LiteralPath "CMakePresets.json") {
                Invoke-LoggedCommand "cmake" @("--preset", "windows-mingw")
            } else {
                Invoke-LoggedCommand "cmake" @("-S", ".", "-B", "build", "-G", "MinGW Makefiles", "-DCMAKE_PREFIX_PATH=C:/Qt/6.11.1/mingw_64")
            }
        }

        if (Test-Path -LiteralPath "CMakePresets.json") {
            Invoke-LoggedCommand "cmake" @("--build", "--preset", "windows-mingw")
        } else {
            Invoke-LoggedCommand "cmake" @("--build", "build")
        }
    } finally {
        Pop-Location
    }

    if (-not (Test-Path -LiteralPath $exe)) {
        throw "FocusHud.exe was not built at: $exe"
    }

    Start-Process -FilePath $exe -WorkingDirectory $buildDir
} catch {
    Add-Content -LiteralPath $logFile -Value $_.Exception.ToString()
    Show-LaunchError $_.Exception.Message
    exit 1
}
