#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>
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
    std::list<Character*> m_players;
    std::list<Doors*> m_doors;
    std::list<Gates*> m_gates;

    std::unique_ptr<ArrowsController> m_arrowsController;
    std::unique_ptr<WASDController> m_wasdController;

    Character* m_hotPlayer;
    Character* m_coldPlayer;

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
    void initializeLevel1();
    void drawBoard();
};

#endif // GAME_H
