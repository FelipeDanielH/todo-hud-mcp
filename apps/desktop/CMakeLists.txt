cmake_minimum_required(VERSION 3.16)
project(FocusHud VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2)

qt_add_executable(FocusHud
    src/main.cpp
    src/app/TaskController.cpp
    src/app/TaskController.h
    src/app/TaskModel.cpp
    src/app/TaskModel.h
)

target_link_libraries(FocusHud PRIVATE
    Qt6::Quick
    Qt6::QuickControls2
)

target_compile_definitions(FocusHud PRIVATE
    SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
