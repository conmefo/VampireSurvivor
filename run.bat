@echo off
setlocal

set "ROOT=%~dp0"
set "SOURCE_DIR=%ROOT%."
set "BUILD_DIR=%ROOT%build"
set "EXE=%BUILD_DIR%\MarioGame.exe"

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G Ninja
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
