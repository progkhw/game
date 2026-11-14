# --- CMake для сборки игры "otara" ---

cmake_minimum_required(VERSION 3.24)

project(otara VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)

add_executable(otara
    src/main.cpp
    src/game.cpp
    src/entity.cpp
    src/player.cpp
    src/sheep.cpp
    src/obstacle.cpp
)

target_include_directories(otara PRIVATE include)
target_link_libraries(otara PRIVATE sfml-graphics sfml-window sfml-system)
