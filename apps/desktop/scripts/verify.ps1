param(
    [switch]$RunApp
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$desktopDir = Resolve-Path -LiteralPath (Join-Path $scriptDir "..")
$buildDir = Join-Path $desktopDir "build"
$qtMingwBin = "C:\Qt\Tools\mingw1310_64\bin"
$qtRuntimeBin = "C:\Qt\6.11.1\mingw_64\bin"

foreach ($path in @($qtMingwBin, $qtRuntimeBin)) {
    if (-not (Test-Path -LiteralPath $path)) {
        throw "Required Qt path not found: $path"
    }
}

$env:PATH = "$qtMingwBin;$qtRuntimeBin;$env:PATH"

if (Test-Path -LiteralPath $buildDir) {
    $resolvedBuild = (Resolve-Path -LiteralPath $buildDir).Path
    if (-not $resolvedBuild.StartsWith($desktopDir.Path, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing to remove build directory outside desktop project: $resolvedBuild"
    }
    Remove-Item -LiteralPath $resolvedBuild -Recurse -Force
}

Push-Location $desktopDir
try {
    if (Test-Path -LiteralPath "CMakePresets.json") {
        cmake --preset windows-mingw
        cmake --build --preset windows-mingw
    } else {
        cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64"
        cmake --build build
    }

    cmake --build build --target FocusHud_qmllint

    if ($RunApp) {
        $exe = Join-Path $buildDir "FocusHud.exe"
        if (-not (Test-Path -LiteralPath $exe)) {
            throw "FocusHud.exe was not built at: $exe"
        }

        $stdout = Join-Path $env:TEMP "focushud-verify-out-$PID.txt"
        $stderr = Join-Path $env:TEMP "focushud-verify-err-$PID.txt"
        $process = Start-Process -FilePath $exe `
            -WorkingDirectory $buildDir `
            -RedirectStandardOutput $stdout `
            -RedirectStandardError $stderr `
            -PassThru

        Start-Sleep -Seconds 3
        if ($process.HasExited -and $process.ExitCode -ne 0) {
            throw "FocusHud.exe exited with code $($process.ExitCode)"
        }

        if (-not $process.HasExited) {
            Stop-Process -Id $process.Id -Force
            $process.WaitForExit()
        }

        $stderrText = if (Test-Path -LiteralPath $stderr) { Get-Content -LiteralPath $stderr -Raw } else { "" }
        if (-not [string]::IsNullOrWhiteSpace($stderrText)) {
            throw "FocusHud.exe wrote to stderr:`n$stderrText"
        }
    }
} finally {
    Pop-Location
}

Write-Host "Desktop verification completed."
