@echo off
REM ============================================================================
REM One-Click Windows Standalone Build Script for V-MacroPlus (Python Edition)
REM Builds dist\V-MacroPlus\V-MacroPlus.exe + v_runtime\ + drivers\
REM Runs as Standard User (asInvoker) by default; pass --uac-admin to force UAC
REM ============================================================================
setlocal
cd /d "%~dp0"

set PYTHON_BIN=python
if exist ".venv\Scripts\python.exe" set PYTHON_BIN=.venv\Scripts\python.exe

echo === [1/2] Running Automated Unit Tests ===
"%PYTHON_BIN%" -m unittest discover -s tests -v
if errorlevel 1 exit /b 1

echo === [2/2] Building V-MacroPlus.exe via build_exe.py ===
"%PYTHON_BIN%" build_exe.py %*
if errorlevel 1 exit /b 1

echo === Build Complete! Executable is in dist\V-MacroPlus\V-MacroPlus.exe ===
endlocal
