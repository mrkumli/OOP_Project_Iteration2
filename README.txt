HOT AND COLD - Co-op Puzzle Platformer

 GAME INTRODUCTION

Hot and Cold is a cooperative puzzle platformer game where two players must work together to reach their respective exit doors.
You can control both characters simultaneously to navigate through hazardous levels filled with lava, water, and deadly goo.

 The Twist:
- Hot (Red Player) dies instantly in WATER but can safely walk through LAVA
- Cold (Blue Player) dies instantly in LAVA but can safely walk through WATER
- Both players die in GOO (green hazard)

 Objective:
Guide both Hot and Cold to their respective colored doors at the top of each level. Both players must reach their doors to win!

 Features:
- 5 challenging levels with increasing difficulty
- Pressure plate mechanics that open gates
- Animated doors that open when players approach
- Cooperative gameplay requiring strategic coordination
- Restart and menu options for seamless gameplay

---

 SYSTEM REQUIREMENTS

- Windows 10/11 (64-bit)
- SFML 3.0.2 libraries
- MinGW-w64 GCC compiler (g++)
- Make utility

---

 INSTALLATION & SETUP

1. Extract the Game Files
   - Unzip the entire project to a folder of your choice.
2. Install SFML 3.0.2
    1. Download SFML 3.0.2 for MinGW (GCC 13.1.0 MinGW - 64-bit) from:
    - https://www.sfml-dev.org/download/sfml/3.0.2/

    2. Extract SFML to a location, for example:
    - C:\libraries\SFML-3.0.2


3. Update the `Makefile` in the project root:
   - Open `Makefile` in a text editor
   - Change line 7 to match your SFML installation path:
     - SFML_DIR = C:/libraries/SFML-3.0.2

3. Copy SFML DLLs
Copy these DLL files from `SFML-3.0.2\bin\` to your project root folder:
- sfml-graphics-3.dll
- sfml-window-3.dll
- sfml-system-3.dll

4. Verify MinGW Installation
Open Command Prompt and run:
    - g++ --version

For "command not found" error, install MinGW-w64 and add it to your system PATH.


<!_______________ COMPILING THE GAME _______________!>

 FIRST WAY:
 Using Make (Recommended)

1. Open Command Prompt in the project directory
2. Run the following commands:

Clean previous builds
    - make clean

Compile the game
    - make
Run the game
    - make run


 SECOND WAY:
 Manual Compilation (Alternative)

If Make is not available, compile manually:


g++ -std=c++17 -Wall -Iinclude -IC:/libraries/SFML-3.0.2/include ^
    -c main.cpp Game.cpp Board.cpp Character.cpp Controller.cpp ^
    Doors.cpp Gates.cpp LevelSelect.cpp

g++ *.o -o game.exe -LC:/libraries/SFML-3.0.2/lib ^
    -lsfml-graphics -lsfml-window -lsfml-system


---

 RUNNING THE GAME

After successful compilation, run:
    - game.exe

Or simply double-click `game.exe` in File Explorer.

---

 GAME CONTROLS

 Hot Player  (Red - Fire Character)
- Arrow Keys: Move Left/Right
- Up Arrow: Jump

 Cold Player  (Blue - Water Character)
- A/D Keys: Move Left/Right
- W Key: Jump

 Universal Controls
- ESC: Quit game
- R: Restart level (when won/lost)
- M: Return to main menu (when won/lost)

----------------------------------------

 HOW TO PLAY

1. Main Menu: Use UP/DOWN arrows to select a level, press ENTER to start
2. Cooperative Gameplay: Control both characters simultaneously
3. Avoid Hazards:
   - Hot dies in WATER (blue)
   - Cold dies in LAVA (red/orange)
   - Both die in GOO (green)
4. Activate Mechanisms: Step on pressure plates to open gates
5. Reach the Doors: Both players must reach their colored doors to win

Level Progression
- Level 1: Test Level
- Level 2: Test Level
- Level 3: First actual playable level
- Level 4: Test level
- Level 5: Final complex level

---

PROJECT STRUCTURE

```
HotAndCold/
├── game.exe              Compiled executable
├── Makefile              Build configuration
├── main.cpp              Entry point
├── Game.cpp              Game logic
├── Board.cpp             Level loading
├── Character.cpp         Player physics
├── Controller.cpp        Input handling
├── Doors.cpp             Door mechanics
├── Gates.cpp             Gate/plate mechanics
├── LevelSelect.cpp       Menu system
├── include/              Header files
│   ├── Game.h
│   ├── Board.h
│   ├── Character.h
│   ├── Controller.h
│   ├── Doors.h
│   ├── Gates.h
│   └── LevelSelect.h
├── data/                 Game assets
│   ├── level1.txt - level5.txt
│   ├── board_textures/   Tile graphics
│   ├── player_images/    Character sprites
│   ├── door_images/      Door graphics
│   ├── gates_and_plates/ Mechanism graphics
│   └── screens/          Menu graphics (irrelevant/just for reference)
└── sfml-*.dll            SFML runtime libraries

---

TROUBLESHOOTING

Game Won't Start
- Error: Missing DLL: Copy SFML DLLs to project folder
- Black Screen: Verify `data/` folder exists with all assets
- Crash on Launch: Check that all level files (level1.txt - level5.txt) are present

Compilation Errors
- SFML not found: Update `SFML_DIR` path in Makefile
- Undefined reference errors: Ensure all .cpp files are listed in Makefile
- Permission denied: Run Command Prompt as Administrator

Gameplay Issues
- Players spawn inside walls: Level file may be corrupted, restore from backup
- Doors won't open: Ensure correct player (Hot/Cold) is approaching correct door
- Can't jump: Players need to be on solid ground (air timer resets on landing)

---

TECHNICAL SPECIFICATIONS

- Engine: SFML 3.0.2 (Simple and Fast Multimedia Library)
- Language: C++17
- Compiler: MinGW-w64 GCC 13.1.0+
- Resolution: 640x480 pixels
- Framerate: 60 FPS (V-Sync enabled)
- Physics: Custom 2D platformer physics with gravity and collision detection

---

CREDITS

Developers: Kumail Rizvi & Burair Hyder
Framework: SFML
Course: Object-Oriented Programming & Design Methodologies
Year: Fall 2025

---

LICENSE

This game is created for educational purposes as part of an Object-Oriented Programming course project.

---

CONTACT & SUPPORT

For bugs, questions, or feedback, please contact: sr10462@st.habib.edu.pk

Enjoy the game and remember: teamwork makes the dream work! 🔥💧


Github Repo Link:
'''
https://github.com/mrkumli/OOP_Project_Iteration2
'''
Interim Demo Repo:
'''
https://github.com/mrkumli/SFML-OOP-Game-Project
'''
