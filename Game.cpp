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

    // Try to load font - use a system font as fallback
    if (!m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Warning: Could not load font" << std::endl;
    }

    m_arrowsController = std::make_unique<ArrowsController>();
    m_wasdController = std::make_unique<WASDController>();

    initializeLevel1();
}

Game::~Game() {
    cleanup();
}

void Game::initializeLevel1() {
    cleanup();

    m_hotPlayer = new Hot(sf::Vector2f(48.0f, 400.0f));
    m_coldPlayer = new Cold(sf::Vector2f(560.0f, 400.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    m_doors.push_back(new FireDoor(sf::Vector2f(48.0f, 32.0f)));
    m_doors.push_back(new WaterDoor(sf::Vector2f(560.0f, 32.0f)));

    // Two pressure plates for the left gate
    std::vector<sf::Vector2f> leftPlates = {
        sf::Vector2f(80.0f, 368.0f),
        sf::Vector2f(112.0f, 368.0f)
    };
    m_gates.push_back(new Gates(sf::Vector2f(200.0f, 192.0f), leftPlates));

    // Two pressure plates for the right gate
    std::vector<sf::Vector2f> rightPlates = {
        sf::Vector2f(488.0f, 368.0f),
        sf::Vector2f(520.0f, 368.0f)
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

    // Check gate collisions AFTER player movement
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

                    if (tile == "2") {
                        fallback.setFillColor(sf::Color(255, 80, 0));
                    } else if (tile == "3") {
                        fallback.setFillColor(sf::Color(0, 120, 255));
                    } else if (tile == "4") {
                        fallback.setFillColor(sf::Color(50, 255, 50));
                    } else {
                        fallback.setFillColor(sf::Color(70, 70, 70));
                    }

                    m_window.draw(fallback);
                }
            }
        }
    }
}

void Game::drawGameStateText() {
    if (m_gameState == GameState::Won) {
        // Victory overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 200));
        overlay.setPosition(sf::Vector2f(70, 140));
        overlay.setFillColor(sf::Color(0, 150, 0, 240));
        overlay.setOutlineColor(sf::Color(255, 215, 0));
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Victory text
        sf::Text titleText(m_font, "LEVEL COMPLETE!", 36);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(sf::Vector2f(150, 170));
        m_window.draw(titleText);

        sf::Text instructionsText(m_font, "Press R to Restart\nPress ESC to Quit", 20);
        instructionsText.setFillColor(sf::Color::White);
        instructionsText.setPosition(sf::Vector2f(220, 240));
        m_window.draw(instructionsText);

    } else if (m_gameState == GameState::Lost) {
        // Game over overlay
        sf::RectangleShape overlay(sf::Vector2f(500, 200));
        overlay.setPosition(sf::Vector2f(70, 140));
        overlay.setFillColor(sf::Color(150, 0, 0, 240));
        overlay.setOutlineColor(sf::Color::White);
        overlay.setOutlineThickness(5.0f);
        m_window.draw(overlay);

        // Game over text
        sf::Text titleText(m_font, "GAME OVER", 36);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(sf::Vector2f(210, 170));
        m_window.draw(titleText);

        sf::Text instructionsText(m_font, "Press R to Restart\nPress ESC to Quit", 20);
        instructionsText.setFillColor(sf::Color::White);
        instructionsText.setPosition(sf::Vector2f(220, 240));
        m_window.draw(instructionsText);
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
    // Gate collision - gates act as solid walls when closed
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue;

        sf::FloatRect gateRect = gate->getGateRect();

        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;

            sf::FloatRect playerRect = player->getRect();
            auto intersection = playerRect.findIntersection(gateRect);

            if (intersection) {
                sf::FloatRect overlap = *intersection;

                // Push player out based on smallest overlap
                if (overlap.size.x < overlap.size.y) {
                    // Horizontal collision
                    if (playerRect.position.x < gateRect.position.x) {
                        // Player on left - push left
                        playerRect.position.x -= overlap.size.x;
                    } else {
                        // Player on right - push right
                        playerRect.position.x += overlap.size.x;
                    }
                } else {
                    // Vertical collision
                    if (playerRect.position.y < gateRect.position.y) {
                        // Player above - push up
                        playerRect.position.y -= overlap.size.y;
                    } else {
                        // Player below - push down
                        playerRect.position.y += overlap.size.y;
                    }
                }

                // Update player position
                player->setRect(playerRect);
            }
        }
    }
}

bool Game::checkWin() {
    bool hotAtFireDoor = false;
    bool coldAtWaterDoor = false;

    // Check if both players are alive first
    if (!m_hotPlayer || m_hotPlayer->isDead()) return false;
    if (!m_coldPlayer || m_coldPlayer->isDead()) return false;

    for (auto* door : m_doors) {
        if (!door->isOpen()) continue;

        sf::FloatRect doorRect = door->getRect();

        // Check hot player at fire door
        if (m_hotPlayer->getRect().findIntersection(doorRect)) {
            if (dynamic_cast<FireDoor*>(door) != nullptr) {
                hotAtFireDoor = true;
            }
        }

        // Check cold player at water door
        if (m_coldPlayer->getRect().findIntersection(doorRect)) {
            if (dynamic_cast<WaterDoor*>(door) != nullptr) {
                coldAtWaterDoor = true;
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
