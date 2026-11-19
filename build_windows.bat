@echo off
setlocal EnableDelayedExpansion

echo ============================================================================
echo   V-MacroPlus v3.9.0 (C++20 / Qt 6) - Windows Standalone Build
echo ============================================================================

:: 1. Locate Qt 6 MinGW installation
if not defined QTDIR (
    if exist "C:\Qt\6.12.0\mingw_64\lib\cmake\Qt6\Qt6Config.cmake" (
        set "QTDIR=C:\Qt\6.12.0\mingw_64"
    ) else (
        for /d %%V in ("C:\Qt\6.*") do (
            if exist "%%V\mingw_64\lib\cmake\Qt6\Qt6Config.cmake" (
                set "QTDIR=%%V\mingw_64"
            )
        )
    )
)

if not exist "%QTDIR%\lib\cmake\Qt6\Qt6Config.cmake" (
    echo [ERROR] Could not find Qt 6 MinGW installation.
    echo Please set QTDIR to your Qt 6 mingw_64 folder, e.g.: C:\Qt\6.12.0\mingw_64
    pause
    exit /b 1
)

:: 2. Prepend Qt bin and Qt Tools MinGW (if available) to PATH
set "PATH=%QTDIR%\bin;%PATH%"
for /d %%M in ("C:\Qt\Tools\mingw*") do (
    if exist "%%M\bin\g++.exe" (
        set "PATH=%%M\bin;%PATH%"
    )
)
if exist "C:\Qt\Tools\Ninja\ninja.exe" (
    set "PATH=C:\Qt\Tools\Ninja;%PATH%"
)

echo [INFO] Using QTDIR: %QTDIR%

:: 3. Clean stale CMake cache if a previous configure failed
if exist "build\CMakeCache.txt" (
    findstr /c:"Qt6_DIR:PATH=Qt6_DIR-NOTFOUND" "build\CMakeCache.txt" >nul 2>nul
    if not errorlevel 1 (
        echo [INFO] Removing stale build cache...
        rmdir /s /q build
    )
)

if not exist build mkdir build

:: 4. Configure with CMake + Ninja
cmake -S . -B build -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QTDIR%"
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: 5. Build
cmake --build build --parallel
if errorlevel 1 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

:: 6. Deploy Qt runtime DLLs
if exist "%QTDIR%\bin\windeployqt.exe" (
    "%QTDIR%\bin\windeployqt.exe" --release --no-translations build\V-MacroPlus.exe
) else (
    where windeployqt >nul 2>nul
    if not errorlevel 1 windeployqt --release --no-translations build\V-MacroPlus.exe
)

echo.
echo [SUCCESS] Build completed! Executable is in: build\V-MacroPlus.exe
pause
