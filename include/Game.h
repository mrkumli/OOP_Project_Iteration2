#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <memory>
#include "Board.h"
#include "Character.h"
#include "Doors.h"
#include "Gates.h"
#include "LevelSelect.h"
#include "Controller.h"

enum class GameState {
    Playing,
    Won,
    Lost
};

class Game {
private:
    sf::RenderWindow m_window;
    Board* m_board;

    std::list<Character*> m_players;
    std::list<Doors*> m_doors;
    std::list<Gates*> m_gates;

    Character* m_hotPlayer;
    Character* m_coldPlayer;

    std::unique_ptr<ArrowsController> m_arrowsController;
    std::unique_ptr<WASDController> m_wasdController;

    GameState m_gameState;
    int m_currentLevel;

    sf::Font m_font;

public:
    Game(int levelNumber = 1);
    ~Game();

    void run();
    void draw();
    void update();
    void checkCollisions();
    void checkDeath();
    bool checkWin();
    bool shouldReturnToMenu() const;

private:
    void handleEvents();
    void cleanup();
    void drawBoard();
    void drawGameStateText();
    void initializeLevel(int levelNumber);
};

#endif // GAME_H
