cmake_minimum_required(VERSION 3.16)
project(FocusHud VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)

qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(FocusHud
    src/main.cpp
    src/application/TaskService.cpp
    src/application/TaskService.h
    src/infrastructure/ApiTaskRepository.cpp
    src/infrastructure/ApiTaskRepository.h
    src/infrastructure/InMemoryTaskRepository.cpp
    src/infrastructure/InMemoryTaskRepository.h
    src/presentation/AppController.cpp
    src/presentation/AppController.h
    src/presentation/TaskListModel.cpp
    src/presentation/TaskListModel.h
    src/presentation/FocusTimerController.cpp
    src/presentation/FocusTimerController.h
)

set_source_files_properties(src/qml/theme/Theme.qml PROPERTIES
    QT_QML_SINGLETON_TYPE TRUE
)

target_include_directories(FocusHud PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/presentation
)

target_compile_options(FocusHud PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

qt_add_qml_module(FocusHud
    URI FocusHUD
    VERSION 0.1
    QML_FILES
        src/qml/Main.qml
        src/qml/components/HudCard.qml
        src/qml/components/TaskItem.qml
        src/qml/components/FocusButton.qml
        src/qml/components/HistoryPanel.qml
        src/qml/theme/Theme.qml
)

target_link_libraries(FocusHud PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
)
