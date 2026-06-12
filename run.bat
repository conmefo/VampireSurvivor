@echo off
setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
set "SOURCE_DIR=%ROOT%."
set "BUILD_DIR=%ROOT%build"
set "EXE=%BUILD_DIR%\VampireSurvivors.exe"
set "TARGET_GENERATOR=Ninja"
set "CACHE=%BUILD_DIR%\CMakeCache.txt"

cmake -Wno-deprecated -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "%TARGET_GENERATOR%"
if errorlevel 1 (
    echo.
    echo CMake configure failed.
    exit /b 1
)

cmake --build "%BUILD_DIR%"
if errorlevel 1 (
    echo.
    echo Build failed.
    exit /b 1
)

"%EXE%"
