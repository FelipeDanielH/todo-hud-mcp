# Focus HUD — Desktop UI

Qt 6 / QML desktop frontend for Focus HUD.

## Prerequisites

- CMake ≥ 3.16
- Qt ≥ 6.2 (Core, Gui, Qml, Quick, QuickControls2)
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

QML is bundled into the binary via `qt_add_qml_module()`. No runtime file paths required.

## Architecture (SOLID, hexagonal)

```
src/
├── main.cpp                          # Composition root (wire DI, start engine)
├── domain/
│   └── Task.h                        # Pure data struct (no QObject, no Qt Quick)
├── application/
│   ├── TaskRepository.h              # Port interface (abstract)
│   └── TaskService.h/.cpp            # Use case orchestration
├── infrastructure/
│   └── InMemoryTaskRepository.h/.cpp # Adapter (mock data, swappable)
├── presentation/
│   ├── AppController.h/.cpp          # Facade for QML (Q_PROPERTY, Q_INVOKABLE)
│   ├── TaskListModel.h/.cpp          # QAbstractListModel (roles: id, title, completed, statusText)
│   └── FocusTimerController.h/.cpp   # 25 min countdown timer
└── qml/
    ├── Main.qml                      # Root HUD window (FocusHUD module)
    ├── components/
    │   ├── HudCard.qml               # Reusable dark card container
    │   ├── TaskItem.qml              # Single task row
    │   └── FocusButton.qml           # Primary action button
    └── theme/
        └── Theme.qml                 # Design tokens (singleton, pragma Singleton)
```

### Layer rules

| Layer | Rules |
|-------|-------|
| `domain/` | Pure C++ structs. Zero Qt Quick, zero QObject. |
| `application/` | Interfaces + services. Depends on `domain/` only. Framework-agnostic. |
| `infrastructure/` | Concrete implementations of ports. Swappable via constructor injection. |
| `presentation/` | QObject-based, QML-aware. Exposes state via `Q_PROPERTY` + `NOTIFY`. |
| `qml/` | Presentation only. No business logic, no HTTP calls. Uses `required property`. |

### SOLID mapping

| Principle | How it's applied |
|-----------|------------------|
| **S** | `TaskListModel` owns data presentation; `FocusTimerController` owns timer; `AppController` orchestrates. |
| **O** | Add `HttpTaskRepository` without touching QML; add views by composing components. |
| **L** | `InMemoryTaskRepository` is a drop-in for any future `TaskRepository` impl. |
| **I** | Each controller exposes only what QML needs; no fat interfaces. |
| **D** | `TaskService` depends on `TaskRepository` interface, not on `InMemoryTaskRepository`. |

### Why C++17

Qt 6 itself compiles with C++17. C++20 features (modules, concepts, coroutines) add no value to this codebase and would limit compiler compatibility (MSVC 2019+ vs 2022+).

## Connecting to the backend (future)

1. Create `infrastructure/HttpTaskRepository` implementing `TaskRepository`
2. Swap in `main.cpp`: `HttpTaskRepository repo` instead of `InMemoryTaskRepository`
3. QML layer requires zero changes

## Switching from desktop to backend

- `app.taskListModel` → provides task data for `Repeater` / `ListView`
- `app.focusTimer` → `.formattedTime`, `.isRunning`, `.start()`, `.stop()`, `.reset()`
- `app.selectTask(id)` → highlights a task as "current"
- `app.completeCurrentTask()` → marks task done, advances to next pending task

## MCP note

The "MCP" badge hints at the backend protocol. A future iteration could surface
connection status and available tools directly in the HUD.
