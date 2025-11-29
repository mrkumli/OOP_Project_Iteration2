@echo off
echo =========================================
echo Hot and Cold - Project Diagnostic Tool
echo =========================================
echo.

set ISSUES_FOUND=0

echo [1] Checking Gates.cpp...
findstr /C:"void Gates::Gates" Gates.cpp >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ❌ ERROR: Gates.cpp appears to be empty or commented
    echo   ACTION: Replace Gates.cpp with the correct implementation
    set ISSUES_FOUND=1
) else (
    findstr /C:"//" Gates.cpp | find "void Gates::Gates" >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo   ❌ ERROR: Gates.cpp code is commented out
        echo   ACTION: Uncomment all code in Gates.cpp
        set ISSUES_FOUND=1
    ) else (
        echo   ✓ Gates.cpp looks good
    )
)
echo.

echo [2] Checking Game.cpp spawn positions...
findstr /C:"50.0f, 400.0f" Game.cpp >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ⚠️  WARNING: Spawn positions may cause immediate death
    echo   CURRENT: Likely spawning at (16.0f, 336.0f)
    echo   SHOULD BE: Around (50.0f, 400.0f)
    echo   ACTION: Update spawn positions in Game::initializeLevel1()
    set ISSUES_FOUND=1
) else (
    echo   ✓ Spawn positions look safe
)
echo.

echo [3] Checking GameState enum...
findstr /C:"enum class GameState" include\Game.h >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ❌ ERROR: GameState enum missing from Game.h
    echo   ACTION: Add GameState enum before Game class
    set ISSUES_FOUND=1
) else (
    echo   ✓ GameState enum found
)
echo.

echo [4] Checking Gates.h for isOpen()...
findstr /C:"isOpen" include\Gates.h >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ⚠️  WARNING: isOpen() method may be missing from Gates.h
    echo   ACTION: Add "bool isOpen() const;" to public methods
    set ISSUES_FOUND=1
) else (
    echo   ✓ isOpen() declaration found
)
echo.

echo [5] Checking level1.txt format...
if not exist "data\level1.txt" (
    echo   ❌ ERROR: level1.txt not found
    set ISSUES_FOUND=1
) else (
    for /f %%A in ('type data\level1.txt ^| find /c /v ""') do set LINES=%%A
    if not "!LINES!"=="30" (
        echo   ⚠️  WARNING: level1.txt has !LINES! rows, expected 30
        echo   ACTION: Verify level file format
        set ISSUES_FOUND=1
    ) else (
        echo   ✓ level1.txt has correct number of rows
    )
)
echo.

echo [6] Checking texture files...
set TEXTURE_MISSING=0
if not exist "data\board_textures\100.png" (
    echo   ⚠️  Missing: data\board_textures\100.png
    set TEXTURE_MISSING=1
)
if not exist "data\board_textures\111.png" (
    echo   ⚠️  Missing: data\board_textures\111.png
    set TEXTURE_MISSING=1
)
if not exist "data\board_textures\2.png" (
    echo   ⚠️  Missing: data\board_textures\2.png
    set TEXTURE_MISSING=1
)
if not exist "data\board_textures\3.png" (
    echo   ⚠️  Missing: data\board_textures\3.png
    set TEXTURE_MISSING=1
)
if not exist "data\board_textures\4.png" (
    echo   ⚠️  Missing: data\board_textures\4.png
    set TEXTURE_MISSING=1
)

if %TEXTURE_MISSING%==1 (
    echo   ACTION: Add missing texture files
    set ISSUES_FOUND=1
) else (
    echo   ✓ Core texture files present
)
echo.

echo [7] Checking player textures...
if not exist "data\player_images\magmaboy.png" (
    echo   ⚠️  Missing: data\player_images\magmaboy.png
) else if not exist "data\player_images\hydrogirl.png" (
    echo   ⚠️  Missing: data\player_images\hydrogirl.png
) else (
    echo   ✓ Player textures present
)
echo.

echo [8] Checking Makefile...
findstr /C:"Gates.cpp" Makefile >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ❌ ERROR: Gates.cpp not listed in Makefile SOURCES
    echo   ACTION: Add Gates.cpp to SOURCES variable
    set ISSUES_FOUND=1
) else (
    echo   ✓ Gates.cpp in Makefile
)
echo.

echo =========================================
if %ISSUES_FOUND%==0 (
    echo ✓ ALL CHECKS PASSED
    echo Your project should build and run correctly!
) else (
    echo ⚠️  ISSUES FOUND - SEE ABOVE
    echo.
    echo PRIORITY FIXES:
    echo 1. Uncomment or replace Gates.cpp
    echo 2. Update spawn positions in Game.cpp
    echo 3. Add GameState enum to Game.h
    echo 4. Update level1.txt if needed
    echo.
    echo See SETUP_AND_FIX_GUIDE.md for detailed instructions
)
echo =========================================
echo.
pause
