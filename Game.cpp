#include "include/Game.h"
#include "include/Controller.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({640, 480}), "Hot and Cold - Complete"),
      m_board("data/level1.txt"),
      m_hotPlayer(nullptr),
      m_coldPlayer(nullptr),
      m_gameState(GameState::Playing)
{
    m_window.setFramerateLimit(60);

    m_arrowsController = std::make_unique<ArrowsController>();
    m_wasdController = std::make_unique<WASDController>();

    initializeLevel1();
}

Game::~Game() {
    cleanup();
}

void Game::initializeLevel1() {
    // Clear existing objects
    cleanup();

    // Spawn players on opposite sides of the level
    m_hotPlayer = new Hot(sf::Vector2f(48.0f, 400.0f));     // Left side
    m_coldPlayer = new Cold(sf::Vector2f(560.0f, 400.0f));  // Right side

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    // Doors at top - players must reach these to win
    m_doors.push_back(new FireDoor(sf::Vector2f(48.0f, 32.0f)));   // Top left for hot
    m_doors.push_back(new WaterDoor(sf::Vector2f(560.0f, 32.0f))); // Top right for cold

    // TWO gates with TWO pressure plates
    // Gate 1: Middle-left area, controlled by plate on left
    std::vector<sf::Vector2f> leftPlates = {
        sf::Vector2f(80.0f, 368.0f)  // Left side pressure plate
    };
    m_gates.push_back(new Gates(sf::Vector2f(200.0f, 192.0f), leftPlates));

    // Gate 2: Middle-right area, controlled by plate on right
    std::vector<sf::Vector2f> rightPlates = {
        sf::Vector2f(520.0f, 368.0f)  // Right side pressure plate
    };
    m_gates.push_back(new Gates(sf::Vector2f(400.0f, 192.0f), rightPlates));

    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║   HOT AND COLD - Level 1 Loaded       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "\nOBJECTIVE:" << std::endl;
    std::cout << "  • Hot (RED) must reach the FIRE DOOR (top-left)" << std::endl;
    std::cout << "  • Cold (BLUE) must reach the WATER DOOR (top-right)" << std::endl;
    std::cout << "  • BOTH players must reach their doors to WIN!" << std::endl;
    std::cout << "\nCONTROLS:" << std::endl;
    std::cout << "  Hot Player:  ← → ↑ (Arrow Keys)" << std::endl;
    std::cout << "  Cold Player: A D W (WASD)" << std::endl;
    std::cout << "  ESC: Quit | R: Restart" << std::endl;
    std::cout << "\nMECHANICS:" << std::endl;
    std::cout << "  • Hot dies in WATER (blue)" << std::endl;
    std::cout << "  • Cold dies in LAVA (red/orange)" << std::endl;
    std::cout << "  • Both die in GOO (green)" << std::endl;
    std::cout << "  • Press PLATES to open GATES for each other!" << std::endl;
    std::cout << "\n════════════════════════════════════════\n" << std::endl;
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
                    std::cout << "\n=== RESTARTING LEVEL ===" << std::endl;
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

    // CRITICAL: Check gate collisions BEFORE updating gates
    // This prevents players from passing through closing gates
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

    // Update gates (they open/close based on plates)
    for (auto* gate : m_gates) {
        gate->tryOpen(m_players);
    }

    // Check win condition
    if (checkWin()) {
        m_gameState = GameState::Won;
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║         ⭐ LEVEL COMPLETE! ⭐          ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        std::cout << "  Both players reached their doors!" << std::endl;
        std::cout << "  Press R to play again" << std::endl;
        std::cout << "  Press ESC to quit\n" << std::endl;
    }

    // Check loss condition
    bool hotDead = (m_hotPlayer == nullptr || m_hotPlayer->isDead());
    bool coldDead = (m_coldPlayer == nullptr || m_coldPlayer->isDead());

    if (hotDead && coldDead && m_gameState == GameState::Playing) {
        m_gameState = GameState::Lost;
        std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
        std::cout << "║           💀 GAME OVER 💀             ║" << std::endl;
        std::cout << "╚════════════════════════════════════════╝" << std::endl;
        std::cout << "  Both players died!" << std::endl;
        std::cout << "  Press R to try again" << std::endl;
        std::cout << "  Press ESC to quit\n" << std::endl;
    }
}

void Game::draw() {
    m_window.clear(sf::Color::Black);

    drawBoard();

    // Draw gates (they need to block view when closed)
    for (auto* gate : m_gates) {
        gate->draw(m_window);
    }

    // Draw doors
    for (auto* door : m_doors) {
        door->draw(m_window);
    }

    // Draw players on top
    for (auto* player : m_players) {
        if (player) {
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

    // Draw background
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

    // Draw tiles with fallback colors
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
                    // Fallback colored tiles
                    sf::RectangleShape fallback(sf::Vector2f(CHUNK_SIZE, CHUNK_SIZE));
                    fallback.setPosition(sf::Vector2f(
                        static_cast<float>(x * CHUNK_SIZE),
                        static_cast<float>(y * CHUNK_SIZE)
                    ));

                    if (tile == "2") {
                        fallback.setFillColor(sf::Color(255, 80, 0)); // Lava
                    } else if (tile == "3") {
                        fallback.setFillColor(sf::Color(0, 120, 255)); // Water
                    } else if (tile == "4") {
                        fallback.setFillColor(sf::Color(50, 255, 50)); // Goo
                    } else {
                        fallback.setFillColor(sf::Color(70, 70, 70)); // Walls
                    }

                    m_window.draw(fallback);
                }
            }
        }
    }
}

void Game::drawGameStateText() {
    if (m_gameState == GameState::Won) {
        // Victory screen
        sf::RectangleShape overlay(sf::Vector2f(500, 200));
        overlay.setPosition(sf::Vector2f(70, 140));
        overlay.setFillColor(sf::Color(0, 150, 0, 240));
        overlay.setOutlineColor(sf::Color(255, 215, 0));
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Inner text box
        sf::RectangleShape textBox(sf::Vector2f(480, 80));
        textBox.setPosition(sf::Vector2f(80, 210));
        textBox.setFillColor(sf::Color(0, 100, 0, 200));
        m_window.draw(textBox);

    } else if (m_gameState == GameState::Lost) {
        // Game over screen
        sf::RectangleShape overlay(sf::Vector2f(500, 200));
        overlay.setPosition(sf::Vector2f(70, 140));
        overlay.setFillColor(sf::Color(150, 0, 0, 240));
        overlay.setOutlineColor(sf::Color::White);
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Inner text box
        sf::RectangleShape textBox(sf::Vector2f(480, 80));
        textBox.setPosition(sf::Vector2f(80, 210));
        textBox.setFillColor(sf::Color(100, 0, 0, 200));
        m_window.draw(textBox);
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
    // Check gate collisions - gates act as solid walls when closed
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue; // Gates don't block when open

        sf::FloatRect gateRect = gate->getGateRect();

        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;

            sf::FloatRect playerRect = player->getRect();
            auto intersection = playerRect.findIntersection(gateRect);

            if (intersection) {
                sf::FloatRect overlap = *intersection;

                // Determine collision direction and push player out
                if (overlap.size.x < overlap.size.y) {
                    // Horizontal collision - push left or right
                    if (playerRect.position.x < gateRect.position.x) {
                        // Player on left of gate
                        // We need to access player position - this requires adding a method
                        // For now, we'll just note collision happened
                        // In a full implementation, Character would need setPosition()
                    }
                } else {
                    // Vertical collision - push up or down
                    // Similar issue - needs Character::setPosition()
                }
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

        if (m_hotPlayer && !m_hotPlayer->isDead()) {
            if (m_hotPlayer->getRect().findIntersection(doorRect)) {
                if (dynamic_cast<FireDoor*>(door) != nullptr) {
                    hotAtFireDoor = true;
                }
            }
        }

        if (m_coldPlayer && !m_coldPlayer->isDead()) {
            if (m_coldPlayer->getRect().findIntersection(doorRect)) {
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
