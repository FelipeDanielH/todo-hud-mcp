# Focus HUD Desktop

Qt 6 / QML desktop frontend for Focus HUD.

## Requirements

Current validated Windows setup:

- Windows
- CMake 3.21+ for `CMakePresets.json`
- Qt 6.11.1 MinGW at `C:/Qt/6.11.1/mingw_64`
- Qt MinGW tools at `C:/Qt/Tools/mingw1310_64/bin`

The desktop target uses C++17, Qt Quick, QML, and Qt Quick Controls 2.

## Project Layout

```text
src/
  main.cpp                         # Composition root
  domain/                          # Pure task data
  application/                     # Task service and repository port
  infrastructure/                  # In-memory repository adapter
  presentation/                    # QObject facade, model, timer controller
  qml/
    Main.qml                       # HUD window
    components/                    # Reusable QML controls
    theme/Theme.qml                # QML singleton design tokens
scripts/
  verify.ps1                       # Local desktop validation
```

## Build With Presets

From `apps/desktop`:

```powershell
cmake --preset windows-mingw
cmake --build --preset windows-mingw
```

The preset sets:

- Generator: `MinGW Makefiles`
- Build directory: `apps/desktop/build`
- `CMAKE_PREFIX_PATH`: `C:/Qt/6.11.1/mingw_64`
- Qt/MinGW bin paths for the configure and build environment

## Manual Fallback

If presets are not available:

```powershell
$env:PATH = "C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.11.1\mingw_64\bin;$env:PATH"
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="C:/Qt/6.11.1/mingw_64"
cmake --build build
```

## Validation

Run QML lint:

```powershell
cmake --build build --target FocusHud_qmllint
```

Run the app:

```powershell
.\build\FocusHud.exe
```

Run the desktop verification script:

```powershell
.\scripts\verify.ps1
.\scripts\verify.ps1 -RunApp
```

The `-RunApp` flag performs a short smoke test and then stops the process so the script does not block automation.

## Current State

- UI is local/mock data only.
- Backend/MCP is not connected yet.
- No persistence, settings, tray icon, or packaging yet.
- QML is bundled into the executable via `qt_add_qml_module()`.
- `AppController` is provided to QML through typed initial properties.

## Known Non-Blocking Warning

CMake may print:

```text
Could NOT find WrapVulkanHeaders (missing: Vulkan_INCLUDE_DIR)
```

This does not block the current Qt Quick desktop build.
