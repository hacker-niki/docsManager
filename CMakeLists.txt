cmake_minimum_required(VERSION 3.5)
project(docsManager)

set(CMAKE_CXX_STANDARD 23)

set(HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/../Manger/Manager.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/../Manager/Generator.h")

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)

qt_standard_project_setup()

qt_add_executable(docsManager app/main.cpp
        app/MainWindow.cpp
        app/MainWindow.h
        app/MainWindow.cpp
        app/MainWindow.h
        app/MainWindow.ui)

add_subdirectory(Manager)

target_link_libraries(docsManager PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)

target_link_libraries(docsManager LINK_PUBLIC docsManagerLib)

set_target_properties(docsManager PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
)