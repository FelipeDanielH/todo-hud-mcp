# Focus HUD — Desktop UI

Qt 6 / QML desktop frontend for Focus HUD.

## Prerequisites

- CMake ≥ 3.16
- Qt 6 (Quick, QuickControls2)
- C++17 compiler

### Windows (Chocolatey)

```powershell
choco install cmake qt6-desktop
```

### Ubuntu / Debian

```bash
sudo apt install cmake build-essential qt6-base-dev qt6-declarative-dev libqt6quickcontrols2-6
```

### macOS (Homebrew)

```bash
brew install cmake qt
```

## Build & Run

```bash
cd apps/desktop
cmake -B build -DCMAKE_PREFIX_PATH=/path/to/qt6
cmake --build build
./build/FocusHud
```

On Windows with Qt installed via installer, set `CMAKE_PREFIX_PATH` to the MSVC toolchain dir, e.g.:

```bash
cmake -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.8.0/msvc2022_64"
cmake --build build
```

## Architecture (SOLID)

```
src/
├── main.cpp                   # Entry point, engine setup
├── app/
│   ├── TaskModel.h/.cpp       # Data layer — QAbstractListModel with mock tasks
│   └── TaskController.h/.cpp  # Controller — timer logic, task actions, exposes state to QML
└── qml/
    ├── Main.qml               # Root view (HUD window)
    ├── components/
    │   ├── HudCard.qml        # Reusable dark card container
    │   ├── TaskItem.qml       # Single task row (title + status indicator)
    │   └── FocusButton.qml    # Primary action button
    └── theme/
        └── Theme.qml          # Design tokens (colors, spacing, radius)
```

### SOLID mapping

| Principle | How it's applied |
|-----------|------------------|
| **S** — Single Responsibility | `TaskModel` owns data; `TaskController` owns behavior; QML owns presentation |
| **O** — Open/Closed | Add new views by composing existing components; add new actions via controller slots |
| **L** — Liskov Substitution | `TaskModel` inherits `QAbstractListModel` — any Qt view can consume it |
| **I** — Interface Segregation | Controller exposes only the properties/slots QML needs; no fat interfaces |
| **D** — Dependency Inversion | QML depends on abstractions (`Q_PROPERTY`, signals, slots), not on concrete C++ internals |

## Connecting to the backend (future)

Replace `TaskModel`'s mock data with a `NetworkTaskModel` that fetches from the NestJS API
or subscribes via the MCP Streamable HTTP endpoint. The QML layer requires zero changes.

## MCP note

The "MCP" badge in the UI header hints at the backend protocol.
A future iteration could surface connection status and available tools directly in the HUD.
