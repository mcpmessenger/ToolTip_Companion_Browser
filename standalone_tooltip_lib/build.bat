@echo off
echo Building NaviGrab ToolTip Library...

REM Set up Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 2>nul
    if errorlevel 1 (
        echo Visual Studio not found! Please install Visual Studio 2019 or 2022.
        pause
        exit /b 1
    )
)

REM Create output directory
if not exist "bin" mkdir bin
if not exist "lib" mkdir lib

REM Compile the library
echo Compiling source files...
cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/navigrab_core.cpp /Fo"navigrab_core.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/proactive_scraper.cpp /Fo"proactive_scraper.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/tooltip_service.cpp /Fo"tooltip_service.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/screenshot_capture.cpp /Fo"screenshot_capture.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/element_detector.cpp /Fo"element_detector.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/dark_mode_manager.cpp /Fo"dark_mode_manager.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/tooltip_view.cpp /Fo"tooltip_view.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/navigrab_integration.cpp /Fo"navigrab_integration.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/tooltip_browser_integration.cpp /Fo"tooltip_browser_integration.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/tooltip_prefs.cpp /Fo"tooltip_prefs.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/ai_integration.cpp /Fo"ai_integration.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/fresh_crawl_button.cpp /Fo"fresh_crawl_button.obj"
if errorlevel 1 goto error

cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   /c src/tooltip_toolbar_integration.cpp /Fo"tooltip_toolbar_integration.obj"
if errorlevel 1 goto error

REM Create the library
echo Creating library...
lib /OUT:"lib\navigrab_tooltip.lib" *.obj
if errorlevel 1 goto error

REM Compile the example
echo Compiling example...
cl /std:c++17 /EHsc /O2 /I"../include" /I"../src" /I"C:\chromium\src\src" ^
   /D"HAVE_CHROMIUM" /D"WIN32" /D"_WINDOWS" /D"NDEBUG" ^
   examples/basic_usage.cpp /link "lib\navigrab_tooltip.lib" /OUT:"bin\tooltip_example.exe"
if errorlevel 1 goto error

echo.
echo ✅ Build completed successfully!
echo.
echo Library: lib\navigrab_tooltip.lib
echo Example: bin\tooltip_example.exe
echo.
echo Run the example:
echo   bin\tooltip_example.exe
echo.
goto end

:error
echo.
echo ❌ Build failed!
echo Check the error messages above.
echo.
pause
exit /b 1

:end
pause
