@echo off
setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
set "SOURCE_DIR=%ROOT%."
set "BUILD_DIR=%ROOT%build"
set "EXE=%BUILD_DIR%\MarioGame.exe"
set "TARGET_GENERATOR=Ninja"
set "CACHE=%BUILD_DIR%\CMakeCache.txt"

if exist "%CACHE%" (
    set "EXISTING_GENERATOR="
    for /f "tokens=2 delims==" %%G in ('findstr /b /c:"CMAKE_GENERATOR:INTERNAL=" "%CACHE%"') do set "EXISTING_GENERATOR=%%G"
    if defined EXISTING_GENERATOR if /i not "!EXISTING_GENERATOR!"=="%TARGET_GENERATOR%" (
        echo Existing CMake build uses "!EXISTING_GENERATOR!"; reconfiguring for "%TARGET_GENERATOR%".
        del /f /q "%CACHE%" >nul 2>nul
        if exist "%BUILD_DIR%\CMakeFiles" rmdir /s /q "%BUILD_DIR%\CMakeFiles"
    )
)

cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%" -G "%TARGET_GENERATOR%"
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
