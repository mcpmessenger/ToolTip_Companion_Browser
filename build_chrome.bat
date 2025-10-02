@echo off
echo Building Chrome Browser with Tooltip Components...
echo.

REM Check if we're in the right directory
if not exist "chrome\browser\tooltip" (
    echo Error: Please run this script from the Chromium source root directory
    echo Expected: C:\chromium\src\src
    pause
    exit /b 1
)

echo Step 1: Building tooltip components...
call .\out\Default\siso_failed_commands.bat
if %errorlevel% neq 0 (
    echo Error: Failed to build tooltip components
    pause
    exit /b 1
)

echo Step 2: Building Chrome browser...
python tools\buildstate.py
if %errorlevel% neq 0 (
    echo Error: Failed to build Chrome browser
    pause
    exit /b 1
)

echo Step 3: Checking for Chrome executable...
if exist "out\Default\chrome.exe" (
    echo SUCCESS: Chrome browser built successfully!
    echo Location: out\Default\chrome.exe
) else (
    echo WARNING: Chrome executable not found
    echo Checking for alternative locations...
    dir out\Default\ | findstr chrome
)

echo.
echo Build completed. Check the output above for results.
pause
