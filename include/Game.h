#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <memory> // Required for std::unique_ptr
#include "Board.h"
#include "Character.h"
#include "Doors.h"
#include "Gates.h"
#include "LevelSelect.h"
#include "Controller.h"

class Game {
private:
    sf::RenderWindow m_window;
    Board m_board;

    // Lists for game objects
    std::list<Character*> m_players;
    std::list<Doors*> m_doors;
    std::list<Gates*> m_gates;

    // Specific pointers to players for easy access
    Character* m_hotPlayer;
    Character* m_coldPlayer;

    // Controllers
    std::unique_ptr<ArrowsController> m_arrowsController;
    std::unique_ptr<WASDController> m_wasdController;

public:
    Game();
    ~Game();

    void run();
    void draw();
    void update();
    void checkCollisions();
    void checkDeath();
    bool checkWin();

private:
    void handleEvents();
    void cleanup();

    // Helper functions used in Game.cpp
    void drawBoard();
    void initializeLevel1();
};

#endif // GAME_H
