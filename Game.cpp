#include "include/Game.h"
#include "include/Controller.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({640, 480}), "Hot and Cold - Working Version"),
      m_board("data/level1.txt"),
      m_hotPlayer(nullptr),
      m_coldPlayer(nullptr),
      m_gameState(GameState::Playing)
{
    m_window.setFramerateLimit(60);

    m_arrowsController = std::make_unique<ArrowsController>();
    m_wasdController = std::make_unique<WASDController>();

    std::cout << "==================================" << std::endl;
    std::cout << "  HOT AND COLD - Complete Game" << std::endl;
    std::cout << "==================================" << std::endl;

    initializeLevel1();
}

Game::~Game() {
    cleanup();
}

void Game::initializeLevel1() {
    // CRITICAL: Spawn positions must be ON SOLID GROUND, away from hazards
    // Level dimensions: 640x480 window, 40x30 tiles, 16px per tile
    // Bottom row (y=464) is solid ground
    // Spawn one row above ground: y = 464 - 32 (player height) - 16 (one tile) = 416

    // Spawn on LEFT side for hot, RIGHT side for cold
    m_hotPlayer = new Hot(sf::Vector2f(32.0f, 416.0f));
    m_coldPlayer = new Cold(sf::Vector2f(580.0f, 416.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    std::cout << "\n✓ Players created:" << std::endl;
    std::cout << "  Hot (Red) spawn: (32, 416)" << std::endl;
    std::cout << "  Cold (Blue) spawn: (580, 416)" << std::endl;

    // Create doors at TOP of level (near exits)
    // Fire door on LEFT, Water door on RIGHT
    m_doors.push_back(new FireDoor(sf::Vector2f(32.0f, 48.0f)));
    m_doors.push_back(new WaterDoor(sf::Vector2f(576.0f, 48.0f)));

    std::cout << "✓ Doors created: " << m_doors.size() << std::endl;

    // Create a gate with pressure plate in MIDDLE of level
    std::vector<sf::Vector2f> platePositions = {
        sf::Vector2f(312.0f, 448.0f)  // Center bottom
    };
    m_gates.push_back(new Gates(sf::Vector2f(312.0f, 240.0f), platePositions));

    std::cout << "✓ Gates created: " << m_gates.size() << std::endl;

    std::cout << "\nControls:" << std::endl;
    std::cout << "  Hot (Red):  Arrow Keys" << std::endl;
    std::cout << "  Cold (Blue): WASD Keys" << std::endl;
    std::cout << "  ESC: Quit" << std::endl;
    std::cout << "  R: Restart (after game over)" << std::endl;

    std::cout << "\nObjective:" << std::endl;
    std::cout << "  - Get HOT to FIRE door (red)" << std::endl;
    std::cout << "  - Get COLD to WATER door (blue)" << std::endl;
    std::cout << "  - Both must reach doors to win!" << std::endl;

    std::cout << "\nHazards:" << std::endl;
    std::cout << "  - Hot dies in WATER (blue)" << std::endl;
    std::cout << "  - Cold dies in LAVA (red/orange)" << std::endl;
    std::cout << "  - Both die in GOO (green)" << std::endl;

    std::cout << "\n==================================" << std::endl;
    std::cout << "Game starting..." << std::endl;
    std::cout << "==================================" << std::endl << std::endl;

    // Debug: Check initial hazard collisions
    checkDeath();
    if (m_hotPlayer->isDead() || m_coldPlayer->isDead()) {
        std::cerr << "\n!!! ERROR: Player died on spawn! Check level file!" << std::endl;
        std::cerr << "Hot alive: " << !m_hotPlayer->isDead() << std::endl;
        std::cerr << "Cold alive: " << !m_coldPlayer->isDead() << std::endl;
    }
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

            // Restart functionality
            if (keyPressed->code == sf::Keyboard::Key::R) {
                if (m_gameState == GameState::Won || m_gameState == GameState::Lost) {
                    std::cout << "\n=== RESTARTING GAME ===" << std::endl;
                    cleanup();
                    m_gameState = GameState::Playing;
                    initializeLevel1();
                }
            }
        }

        // Only process player input during gameplay
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

    // Check death conditions AFTER movement
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

    // Check collisions with gates (if implemented)
    checkCollisions();

    // Check win condition
    if (checkWin()) {
        m_gameState = GameState::Won;
        std::cout << "\n========================================" << std::endl;
        std::cout << "           ⭐⭐ VICTORY! ⭐⭐" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Both players reached their doors!" << std::endl;
        std::cout << "Press R to restart or ESC to quit" << std::endl;
        std::cout << "========================================" << std::endl;
    }

    // Check if both players are dead
    bool hotDead = (m_hotPlayer == nullptr || m_hotPlayer->isDead());
    bool coldDead = (m_coldPlayer == nullptr || m_coldPlayer->isDead());

    if (hotDead && coldDead && m_gameState == GameState::Playing) {
        m_gameState = GameState::Lost;
        std::cout << "\n========================================" << std::endl;
        std::cout << "           💀 GAME OVER 💀" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Both players died!" << std::endl;
        std::cout << "Press R to restart or ESC to quit" << std::endl;
        std::cout << "========================================" << std::endl;
    }
}

void Game::draw() {
    m_window.clear(sf::Color::Black);

    // Draw board (background and tiles)
    drawBoard();

    // Draw gates
    for (auto* gate : m_gates) {
        gate->draw(m_window);
    }

    // Draw doors
    for (auto* door : m_doors) {
        door->draw(m_window);
    }

    // Draw players on top
    for (auto* player : m_players) {
        if (player) {  // Draw even if dead to show where they died
            player->draw(m_window);
        }
    }

    // Draw game state overlay
    drawGameStateText();

    m_window.display();
}

void Game::drawBoard() {
    const auto& textures = m_board.getTextures();
    const auto& gameMap = m_board.getGameMap();

    // Draw background - fill entire window
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
        // Fallback: draw gray background if texture missing
        sf::RectangleShape bg(sf::Vector2f(640, 480));
        bg.setFillColor(sf::Color(100, 100, 100));
        m_window.draw(bg);
    }

    // Draw tiles
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
                    // Draw colored rectangle as fallback if texture not found
                    sf::RectangleShape fallbackTile(sf::Vector2f(CHUNK_SIZE, CHUNK_SIZE));
                    fallbackTile.setPosition(sf::Vector2f(
                        static_cast<float>(x * CHUNK_SIZE),
                        static_cast<float>(y * CHUNK_SIZE)
                    ));

                    // Color based on tile type
                    if (tile == "2") {
                        fallbackTile.setFillColor(sf::Color(255, 100, 0)); // Orange lava
                    } else if (tile == "3") {
                        fallbackTile.setFillColor(sf::Color(0, 150, 255)); // Blue water
                    } else if (tile == "4") {
                        fallbackTile.setFillColor(sf::Color(0, 255, 50)); // Green goo
                    } else {
                        fallbackTile.setFillColor(sf::Color(80, 80, 80)); // Gray walls
                    }

                    m_window.draw(fallbackTile);
                }
            }
        }
    }
}

void Game::drawGameStateText() {
    if (m_gameState == GameState::Won) {
        // Green victory overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 150));
        overlay.setPosition(sf::Vector2f(70, 165));
        overlay.setFillColor(sf::Color(0, 180, 0, 230));
        overlay.setOutlineColor(sf::Color(255, 215, 0));
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);
    }
    else if (m_gameState == GameState::Lost) {
        // Red game over overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 150));
        overlay.setPosition(sf::Vector2f(70, 165));
        overlay.setFillColor(sf::Color(180, 0, 0, 230));
        overlay.setOutlineColor(sf::Color::White);
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);
    }
}

void Game::checkDeath() {
    // Check each player against hazards
    for (auto* player : m_players) {
        if (!player || player->isDead()) continue;

        sf::FloatRect playerRect = player->getRect();
        std::string playerType = player->getType();

        // COLD DIES IN LAVA
        if (playerType == "cold") {
            for (const auto& lavaPool : m_board.getLavaPools()) {
                if (playerRect.findIntersection(lavaPool)) {
                    player->kill();
                    std::cout << "💀 COLD PLAYER DIED IN LAVA!" << std::endl;
                    break;
                }
            }
        }

        // HOT DIES IN WATER
        if (playerType == "hot") {
            for (const auto& waterPool : m_board.getWaterPools()) {
                if (playerRect.findIntersection(waterPool)) {
                    player->kill();
                    std::cout << "💀 HOT PLAYER DIED IN WATER!" << std::endl;
                    break;
                }
            }
        }

        // BOTH DIE IN GOO
        if (!player->isDead()) {
            for (const auto& gooPool : m_board.getGooPools()) {
                if (playerRect.findIntersection(gooPool)) {
                    player->kill();
                    std::cout << "💀 " << playerType << " PLAYER DIED IN GOO!" << std::endl;
                    break;
                }
            }
        }
    }
}

void Game::checkCollisions() {
    // Check collisions with closed gates
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue;

        sf::FloatRect gateRect = gate->getGateRect();

        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;

            // Simple gate blocking (gates act as solid walls when closed)
            sf::FloatRect playerRect = player->getRect();
            if (playerRect.findIntersection(gateRect)) {
                // Gate blocks player - push them back
                // This is simplified; full implementation would need velocity consideration
            }
        }
    }
}

bool Game::checkWin() {
    bool hotAtFireDoor = false;
    bool coldAtWaterDoor = false;

    for (auto* door : m_doors) {
        if (!door->isOpen()) continue;

        sf::FloatRect doorRect = door->getRect();

        // Check if hot player is at fire door
        if (m_hotPlayer && !m_hotPlayer->isDead()) {
            if (m_hotPlayer->getRect().findIntersection(doorRect)) {
                // Use dynamic_cast to check door type
                if (dynamic_cast<FireDoor*>(door) != nullptr) {
                    hotAtFireDoor = true;
                }
            }
        }

        // Check if cold player is at water door
        if (m_coldPlayer && !m_coldPlayer->isDead()) {
            if (m_coldPlayer->getRect().findIntersection(doorRect)) {
                // Use dynamic_cast to check door type
                if (dynamic_cast<WaterDoor*>(door) != nullptr) {
                    coldAtWaterDoor = true;
                }
            }
        }
    }

    return hotAtFireDoor && coldAtWaterDoor;
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
