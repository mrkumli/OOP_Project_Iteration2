@echo off
echo ========================================
echo Hot and Cold - Build Verification
echo ========================================
echo.

echo [1/6] Checking file structure...
set MISSING=0

if not exist "Board.cpp" (
    echo   ERROR: Board.cpp missing
    set MISSING=1
)
if not exist "Character.cpp" (
    echo   ERROR: Character.cpp missing
    set MISSING=1
)
if not exist "Game.cpp" (
    echo   ERROR: Game.cpp missing
    set MISSING=1
)
if not exist "include\Board.h" (
    echo   ERROR: include\Board.h missing
    set MISSING=1
)
if not exist "data\level1.txt" (
    echo   ERROR: data\level1.txt missing
    set MISSING=1
)

if %MISSING%==0 (
    echo   OK: All source files present
) else (
    echo   FAILED: Missing files detected
    goto :error
)
echo.

echo [2/6] Checking SFML DLLs...
set DLLS_OK=1

if not exist "sfml-graphics-3.dll" (
    echo   WARNING: sfml-graphics-3.dll not found
    set DLLS_OK=0
)
if not exist "sfml-window-3.dll" (
    echo   WARNING: sfml-window-3.dll not found
    set DLLS_OK=0
)
if not exist "sfml-system-3.dll" (
    echo   WARNING: sfml-system-3.dll not found
    set DLLS_OK=0
)

if %DLLS_OK%==1 (
    echo   OK: All SFML DLLs present
) else (
    echo   WARNING: Copy DLLs from SFML\bin folder
)
echo.

echo [3/6] Checking data folder structure...
if exist "data\board_textures" (
    echo   OK: board_textures folder exists
) else (
    echo   WARNING: data\board_textures missing
)
if exist "data\player_images" (
    echo   OK: player_images folder exists
) else (
    echo   WARNING: data\player_images missing
)
if exist "data\door_images" (
    echo   OK: door_images folder exists
) else (
    echo   WARNING: data\door_images missing
)
echo.

echo [4/6] Checking for compiler...
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo   ERROR: g++ compiler not found in PATH
    goto :error
) else (
    echo   OK: g++ compiler found
)
echo.

echo [5/6] Attempting to build...
echo   Running: make clean
make clean >nul 2>&1

echo   Running: make
make 2>build_errors.txt
if %ERRORLEVEL% NEQ 0 (
    echo   ERROR: Build failed! Check build_errors.txt
    type build_errors.txt
    goto :error
) else (
    echo   OK: Build successful!
    del build_errors.txt >nul 2>&1
)
echo.

echo [6/6] Verifying executable...
if exist "game.exe" (
    echo   OK: game.exe created successfully
) else (
    echo   ERROR: game.exe not found
    goto :error
)
echo.

echo ========================================
echo BUILD VERIFICATION PASSED!
echo ========================================
echo.
echo You can now run the game with: game.exe
echo Or use: make run
echo.
echo Quick Test Checklist:
echo   1. Both players should appear on screen
echo   2. Arrow keys move red player
echo   3. WASD keys move blue player
echo   4. Players should collide with walls
echo   5. Press ESC to quit
echo.
pause
exit /b 0

:error
echo.
echo ========================================
echo BUILD VERIFICATION FAILED
echo ========================================
echo.
echo Please check the errors above and fix them.
echo Refer to IMPLEMENTATION_GUIDE.md for help.
echo.
pause
exit /b 1
