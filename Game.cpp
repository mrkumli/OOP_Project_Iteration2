#include "include/Game.h"
#include "include/Controller.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({640, 480}), "Hot and Cold - Final"),
      m_board("data/level1.txt"),
      m_hotPlayer(nullptr),
      m_coldPlayer(nullptr),
      m_gameState(GameState::Playing),
      m_font()  // Initialize font
{
    m_window.setFramerateLimit(60);

    m_arrowsController = std::make_unique<ArrowsController>();
    m_wasdController = std::make_unique<WASDController>();

    // Try to load font for text rendering
    if (!m_font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Warning: Could not load font" << std::endl;
    }

    initializeLevel1();
}

Game::~Game() {
    cleanup();
}

void Game::initializeLevel1() {
    cleanup();

    // Players spawn at BOTTOM LEFT (together)
    m_hotPlayer = new Hot(sf::Vector2f(32.0f, 416.0f));
    m_coldPlayer = new Cold(sf::Vector2f(64.0f, 416.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    // Doors at TOP LEFT (end of maze)
    m_doors.push_back(new FireDoor(sf::Vector2f(32.0f, 48.0f)));
    m_doors.push_back(new WaterDoor(sf::Vector2f(64.0f, 48.0f)));

    // ONE gate in middle of maze with TWO buttons
    std::vector<sf::Vector2f> gatePlates = {
        sf::Vector2f(160.0f, 320.0f),  // Button before gate
        sf::Vector2f(400.0f, 320.0f)   // Button after gate
    };
    m_gates.push_back(new Gates(sf::Vector2f(280.0f, 192.0f), gatePlates));

    std::cout << "\n╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║     HOT AND COLD - Level 1 Start        ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    std::cout << "\nGOAL:" << std::endl;
    std::cout << "  Navigate to TOP-LEFT doors" << std::endl;
    std::cout << "  Both players must reach doors to WIN" << std::endl;
    std::cout << "\nCONTROLS:" << std::endl;
    std::cout << "  Hot:  Arrow Keys (←→↑)" << std::endl;
    std::cout << "  Cold: WASD (A D W)" << std::endl;
    std::cout << "  ESC: Quit | R: Restart" << std::endl;
    std::cout << "\nRULES:" << std::endl;
    std::cout << "  • Hot dies in WATER (blue)" << std::endl;
    std::cout << "  • Cold dies in LAVA (red)" << std::endl;
    std::cout << "  • Both die in GOO (green)" << std::endl;
    std::cout << "  • Stand on BUTTONS to open GATE" << std::endl;
    std::cout << "  • Work together!" << std::endl;
    std::cout << "\n══════════════════════════════════════════\n" << std::endl;
}

void Game::run() {
    while (m_window.isOpen()) {
        handleEvents();
        update();
        draw();
    }
}

void Game::handleEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Escape) {
                m_window.close();
            }

            if (keyPressed->code == sf::Keyboard::Key::R) {
                if (m_gameState == GameState::Won || m_gameState == GameState::Lost) {
                    std::cout << "\n=== RESTARTING ===\n" << std::endl;
                    m_gameState = GameState::Playing;
                    initializeLevel1();
                }
            }
        }

        if (m_gameState == GameState::Playing) {
            if (m_hotPlayer && !m_hotPlayer->isDead()) {
                m_arrowsController->controlPlayer(*event, *m_hotPlayer);
            }
            if (m_coldPlayer && !m_coldPlayer->isDead()) {
                m_wasdController->controlPlayer(*event, *m_coldPlayer);
            }
        }
    }
}

void Game::update() {
    if (m_gameState != GameState::Playing) return;

    // Update players
    for (auto* player : m_players) {
        if (player && !player->isDead()) {
            player->update(m_board);
        }
    }

    // CRITICAL: Check gate collisions AFTER player movement
    // This makes gates act as solid walls
    checkCollisions();

    checkDeath();

    // Update doors
    for (auto* door : m_doors) {
        for (auto* player : m_players) {
            if (player && !player->isDead()) {
                door->tryOpen(*player);
            }
        }
    }

    // Update gates
    for (auto* gate : m_gates) {
        gate->tryOpen(m_players);
    }

    // Check win condition
    if (checkWin()) {
        m_gameState = GameState::Won;
        std::cout << "\n╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║          🎉 LEVEL COMPLETE! 🎉          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
        std::cout << "  Both players reached the doors!" << std::endl;
        std::cout << "  Press R to play again\n" << std::endl;
    }

    // Check loss condition
    bool hotDead = (m_hotPlayer == nullptr || m_hotPlayer->isDead());
    bool coldDead = (m_coldPlayer == nullptr || m_coldPlayer->isDead());

    if (hotDead && coldDead && m_gameState == GameState::Playing) {
        m_gameState = GameState::Lost;
        std::cout << "\n╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║            💀 GAME OVER 💀              ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
        std::cout << "  Both players died!" << std::endl;
        std::cout << "  Press R to try again\n" << std::endl;
    }
}

void Game::draw() {
    m_window.clear(sf::Color::Black);

    drawBoard();

    for (auto* gate : m_gates) {
        gate->draw(m_window);
    }

    for (auto* door : m_doors) {
        door->draw(m_window);
    }

    for (auto* player : m_players) {
        if (player) {
            player->draw(m_window);
        }
    }

    drawGameStateText();

    m_window.display();
}

void Game::drawBoard() {
    const auto& textures = m_board.getTextures();
    const auto& gameMap = m_board.getGameMap();

    auto bgIt = textures.find("background");
    if (bgIt != textures.end()) {
        sf::Sprite bgSprite(bgIt->second);
        sf::Vector2u windowSize = m_window.getSize();
        sf::FloatRect spriteSize = bgSprite.getLocalBounds();

        bgSprite.setScale(sf::Vector2f(
            static_cast<float>(windowSize.x) / spriteSize.size.x,
            static_cast<float>(windowSize.y) / spriteSize.size.y
        ));
        m_window.draw(bgSprite);
    } else {
        sf::RectangleShape bg(sf::Vector2f(640, 480));
        bg.setFillColor(sf::Color(100, 100, 100));
        m_window.draw(bg);
    }

    const int CHUNK_SIZE = 16;
    for (size_t y = 0; y < gameMap.size(); ++y) {
        for (size_t x = 0; x < gameMap[y].size(); ++x) {
            const std::string& tile = gameMap[y][x];

            if (tile != "0") {
                auto it = textures.find(tile);
                if (it != textures.end()) {
                    sf::Sprite tileSprite(it->second);
                    tileSprite.setPosition(sf::Vector2f(
                        static_cast<float>(x * CHUNK_SIZE),
                        static_cast<float>(y * CHUNK_SIZE)
                    ));
                    m_window.draw(tileSprite);
                } else {
                    sf::RectangleShape fallback(sf::Vector2f(CHUNK_SIZE, CHUNK_SIZE));
                    fallback.setPosition(sf::Vector2f(
                        static_cast<float>(x * CHUNK_SIZE),
                        static_cast<float>(y * CHUNK_SIZE)
                    ));

                    if (tile == "2") fallback.setFillColor(sf::Color(255, 80, 0));
                    else if (tile == "3") fallback.setFillColor(sf::Color(0, 120, 255));
                    else if (tile == "4") fallback.setFillColor(sf::Color(50, 255, 50));
                    else fallback.setFillColor(sf::Color(70, 70, 70));

                    m_window.draw(fallback);
                }
            }
        }
    }
}

void Game::drawGameStateText() {
    if (m_gameState == GameState::Won) {
        // Green victory overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 250));
        overlay.setPosition(sf::Vector2f(70, 115));
        overlay.setFillColor(sf::Color(0, 150, 0, 240));
        overlay.setOutlineColor(sf::Color(255, 215, 0));
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Text
        sf::Text titleText("LEVEL COMPLETE!", m_font, 36);
        titleText.setPosition(sf::Vector2f(140, 160));
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        m_window.draw(titleText);

        sf::Text infoText("Both players reached the doors!", m_font, 20);
        infoText.setPosition(sf::Vector2f(130, 220));
        infoText.setFillColor(sf::Color::White);
        m_window.draw(infoText);

        sf::Text restartText("Press R to play again", m_font, 18);
        restartText.setPosition(sf::Vector2f(180, 280));
        restartText.setFillColor(sf::Color(255, 255, 0));
        m_window.draw(restartText);

        sf::Text quitText("Press ESC to quit", m_font, 18);
        quitText.setPosition(sf::Vector2f(200, 310));
        quitText.setFillColor(sf::Color(200, 200, 200));
        m_window.draw(quitText);

    } else if (m_gameState == GameState::Lost) {
        // Red game over overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 250));
        overlay.setPosition(sf::Vector2f(70, 115));
        overlay.setFillColor(sf::Color(150, 0, 0, 240));
        overlay.setOutlineColor(sf::Color::White);
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Text
        sf::Text titleText("GAME OVER", m_font, 48);
        titleText.setPosition(sf::Vector2f(180, 150));
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        m_window.draw(titleText);

        sf::Text infoText("Both players died!", m_font, 24);
        infoText.setPosition(sf::Vector2f(200, 220));
        infoText.setFillColor(sf::Color::White);
        m_window.draw(infoText);

        sf::Text restartText("Press R to try again", m_font, 20);
        restartText.setPosition(sf::Vector2f(185, 270));
        restartText.setFillColor(sf::Color(255, 100, 100));
        m_window.draw(restartText);

        sf::Text quitText("Press ESC to quit", m_font, 18);
        quitText.setPosition(sf::Vector2f(200, 310));
        quitText.setFillColor(sf::Color(200, 200, 200));
        m_window.draw(quitText);
    }
}

void Game::checkDeath() {
    for (auto* player : m_players) {
        if (!player || player->isDead()) continue;

        sf::FloatRect playerRect = player->getRect();
        std::string playerType = player->getType();

        if (playerType == "cold") {
            for (const auto& lava : m_board.getLavaPools()) {
                if (playerRect.findIntersection(lava)) {
                    player->kill();
                    std::cout << "💀 COLD died in LAVA!" << std::endl;
                    break;
                }
            }
        }

        if (playerType == "hot") {
            for (const auto& water : m_board.getWaterPools()) {
                if (playerRect.findIntersection(water)) {
                    player->kill();
                    std::cout << "💀 HOT died in WATER!" << std::endl;
                    break;
                }
            }
        }

        if (!player->isDead()) {
            for (const auto& goo : m_board.getGooPools()) {
                if (playerRect.findIntersection(goo)) {
                    player->kill();
                    std::cout << "💀 " << playerType << " died in GOO!" << std::endl;
                    break;
                }
            }
        }
    }
}

void Game::checkCollisions() {
    // GATE COLLISION - Gates act as solid walls when closed
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue; // Open gates don't block

        sf::FloatRect gateRect = gate->getGateRect();

        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;

            sf::FloatRect playerRect = player->getRect();
            auto intersection = playerRect.findIntersection(gateRect);

            if (intersection) {
                sf::FloatRect overlap = *intersection;

                // Push player out of gate
                if (overlap.size.x < overlap.size.y) {
                    // Horizontal collision - push left or right
                    if (playerRect.position.x < gateRect.position.x) {
                        // Player on left, push left
                        sf::Vector2f newPos = playerRect.position;
                        newPos.x -= overlap.size.x;
                        player->setPosition(newPos);
                    } else {
                        // Player on right, push right
                        sf::Vector2f newPos = playerRect.position;
                        newPos.x += overlap.size.x;
                        player->setPosition(newPos);
                    }
                } else {
                    // Vertical collision - push up or down
                    if (playerRect.position.y < gateRect.position.y) {
                        // Player above, push up
                        sf::Vector2f newPos = playerRect.position;
                        newPos.y -= overlap.size.y;
                        player->setPosition(newPos);
                    } else {
                        // Player below, push down
                        sf::Vector2f newPos = playerRect.position;
                        newPos.y += overlap.size.y;
                        player->setPosition(newPos);
                    }
                }
            }
        }
    }
}

bool Game::checkWin() {
    // BOTH players must be at their doors AND doors must be open
    bool hotAtDoor = false;
    bool coldAtDoor = false;

    // Check if players are at doors
    for (auto* door : m_doors) {
        if (!door->isOpen()) continue; // Door must be open

        sf::FloatRect doorRect = door->getRect();

        // Check hot player at fire door
        if (m_hotPlayer && !m_hotPlayer->isDead()) {
            if (m_hotPlayer->getRect().findIntersection(doorRect)) {
                if (dynamic_cast<FireDoor*>(door) != nullptr) {
                    hotAtDoor = true;
                    std::cout << "✓ Hot at Fire Door" << std::endl;
                }
            }
        }

        // Check cold player at water door
        if (m_coldPlayer && !m_coldPlayer->isDead()) {
            if (m_coldPlayer->getRect().findIntersection(doorRect)) {
                if (dynamic_cast<WaterDoor*>(door) != nullptr) {
                    coldAtDoor = true;
                    std::cout << "✓ Cold at Water Door" << std::endl;
                }
            }
        }
    }

    // BOTH must be at doors to win
    if (hotAtDoor && coldAtDoor) {
        std::cout << ">>> WIN CONDITION MET <<<" << std::endl;
        return true;
    }

    return false;
}

void Game::cleanup() {
    for (auto* player : m_players) {
        delete player;
    }
    m_players.clear();

    for (auto* door : m_doors) {
        delete door;
    }
    m_doors.clear();

    for (auto* gate : m_gates) {
        delete gate;
    }
    m_gates.clear();

    m_hotPlayer = nullptr;
    m_coldPlayer = nullptr;
}
