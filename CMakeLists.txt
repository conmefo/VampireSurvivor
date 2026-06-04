cmake_minimum_required(VERSION 3.20)

project(MarioGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include(FetchContent)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(SFML_BUILD_AUDIO OFF CACHE BOOL "Build SFML audio module" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "Build SFML network module" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "Build SFML examples" FORCE)
set(SFML_BUILD_DOC OFF CACHE BOOL "Build SFML documentation" FORCE)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SFML)

add_executable(MarioGame
    src/main.cpp
)

target_include_directories(MarioGame PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(MarioGame PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
)
