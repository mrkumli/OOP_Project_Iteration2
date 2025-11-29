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

    // Load font
    if (!m_font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Warning: Could not load font" << std::endl;
    }

    initializeLevel1();
}

Game::~Game() {
    cleanup();
}

void Game::initializeLevel1() {
    cleanup();

    m_hotPlayer = new Hot(sf::Vector2f(32.0f, 416.0f));
    m_coldPlayer = new Cold(sf::Vector2f(64.0f, 416.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    m_doors.push_back(new FireDoor(sf::Vector2f(32.0f, 48.0f)));
    m_doors.push_back(new WaterDoor(sf::Vector2f(64.0f, 48.0f)));

    std::vector<sf::Vector2f> gatePlates = {
        sf::Vector2f(160.0f, 320.0f),
        sf::Vector2f(400.0f, 320.0f)
    };
    m_gates.push_back(new Gates(sf::Vector2f(280.0f, 192.0f), gatePlates));

    std::cout << "\n╔══════════════════════════════════════════╗\n"
              << "║     HOT AND COLD - Level 1 Start        ║\n"
              << "╚══════════════════════════════════════════╝\n";
    std::cout << "GOAL: Navigate to TOP-LEFT doors\n";
    std::cout << "CONTROLS:\n  Hot: Arrow Keys (←→↑)\n  Cold: WASD (A D W)\n  ESC: Quit | R: Restart\n";
    std::cout << "RULES:\n  • Hot dies in WATER (blue)\n  • Cold dies in LAVA (red)\n"
              << "  • Both die in GOO (green)\n  • Stand on BUTTONS to open GATE\n  • Work together!\n";
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
                    std::cout << "\n=== RESTARTING ===\n";
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

    for (auto* player : m_players) {
        if (player && !player->isDead()) player->update(m_board);
    }

    checkCollisions();
    checkDeath();

    for (auto* door : m_doors) {
        for (auto* player : m_players) {
            if (player && !player->isDead()) door->tryOpen(*player);
        }
    }

    for (auto* gate : m_gates) {
        gate->tryOpen(m_players);
    }

    if (checkWin()) {
        m_gameState = GameState::Won;
        std::cout << "\n╔══════════════════════════════════════════╗\n"
                  << "║          🎉 LEVEL COMPLETE! 🎉          ║\n"
                  << "╚══════════════════════════════════════════╝\n"
                  << "Both players reached the doors! Press R to play again\n";
    }

    bool hotDead = !m_hotPlayer || m_hotPlayer->isDead();
    bool coldDead = !m_coldPlayer || m_coldPlayer->isDead();
    if (hotDead && coldDead && m_gameState == GameState::Playing) {
        m_gameState = GameState::Lost;
        std::cout << "\n╔══════════════════════════════════════════╗\n"
                  << "║            💀 GAME OVER 💀              ║\n"
                  << "╚══════════════════════════════════════════╝\n"
                  << "Both players died! Press R to try again\n";
    }
}

void Game::draw() {
    m_window.clear(sf::Color::Black);

    drawBoard();

    for (auto* gate : m_gates) gate->draw(m_window);
    for (auto* door : m_doors) door->draw(m_window);
    for (auto* player : m_players) if (player) player->draw(m_window);

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
        sf::RectangleShape bg(sf::Vector2f(640.f, 480.f));
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
                    tileSprite.setPosition(sf::Vector2f(x * CHUNK_SIZE, y * CHUNK_SIZE));
                    m_window.draw(tileSprite);
                } else {
                    sf::RectangleShape fallback(sf::Vector2f(CHUNK_SIZE, CHUNK_SIZE));
                    fallback.setPosition(sf::Vector2f(x * CHUNK_SIZE, y * CHUNK_SIZE));

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
    if (m_gameState == GameState::Won || m_gameState == GameState::Lost) {
        sf::RectangleShape overlay(sf::Vector2f(500.f, 250.f));
        overlay.setPosition(sf::Vector2f(70.f, 115.f));
        overlay.setFillColor(m_gameState == GameState::Won ? sf::Color(0, 150, 0, 240) : sf::Color(150, 0, 0, 240));
        overlay.setOutlineColor(m_gameState == GameState::Won ? sf::Color(255, 215, 0) : sf::Color::White);
        overlay.setOutlineThickness(5.f);
        m_window.draw(overlay);

        sf::Text titleText(m_font);
        titleText.setString(m_gameState == GameState::Won ? "LEVEL COMPLETE!" : "GAME OVER");
        titleText.setCharacterSize(36);
        titleText.setFillColor(sf::Color::White);
        titleText.setStyle(sf::Text::Bold);
        titleText.setPosition(sf::Vector2f(140.f, 160.f));
        m_window.draw(titleText);

        sf::Text infoText(m_font);
        infoText.setString(m_gameState == GameState::Won ? "Both players reached the doors!" : "Both players died!");
        infoText.setCharacterSize(24);
        infoText.setFillColor(sf::Color::White);
        infoText.setPosition(sf::Vector2f(130.f, 220.f));
        m_window.draw(infoText);

        sf::Text restartText(m_font);
        restartText.setString(m_gameState == GameState::Won ? "Press R to play again" : "Press R to try again");
        restartText.setCharacterSize(20);
        restartText.setFillColor(sf::Color::Yellow);
        restartText.setPosition(sf::Vector2f(180.f, 280.f));
        m_window.draw(restartText);

        sf::Text quitText(m_font);
        quitText.setString("Press ESC to quit");
        quitText.setCharacterSize(18);
        quitText.setFillColor(sf::Color(200, 200, 200));
        quitText.setPosition(sf::Vector2f(200.f, 310.f));
        m_window.draw(quitText);
    }
}

void Game::checkDeath() {
    for (auto* player : m_players) {
        if (!player || player->isDead()) continue;
        sf::FloatRect playerRect = player->getRect();
        std::string playerType = player->getType();

        if (playerType == "cold") {
            for (const auto& lava : m_board.getLavaPools())
                if (playerRect.findIntersection(lava)) { player->kill(); std::cout << "💀 COLD died in LAVA!\n"; break; }
        }
        if (playerType == "hot") {
            for (const auto& water : m_board.getWaterPools())
                if (playerRect.findIntersection(water)) { player->kill(); std::cout << "💀 HOT died in WATER!\n"; break; }
        }
        if (!player->isDead()) {
            for (const auto& goo : m_board.getGooPools())
                if (playerRect.findIntersection(goo)) { player->kill(); std::cout << "💀 " << playerType << " died in GOO!\n"; break; }
        }
    }
}

void Game::checkCollisions() {
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue;

        sf::FloatRect gateRect = gate->getGateRect();
        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;

            sf::FloatRect playerRect = player->getRect();
            auto intersection = playerRect.findIntersection(gateRect);
            if (!intersection) continue;

            sf::FloatRect overlap = *intersection;
            sf::Vector2f newPos = playerRect.position;

            if (overlap.size.x < overlap.size.y) {
                newPos.x += (playerRect.position.x < gateRect.position.x ? -overlap.size.x : overlap.size.x);
            } else {
                newPos.y += (playerRect.position.y < gateRect.position.y ? -overlap.size.y : overlap.size.y);
            }
            player->setPosition(newPos);
        }
    }
}

bool Game::checkWin() {
    bool hotAtDoor = false;
    bool coldAtDoor = false;

    for (auto* door : m_doors) {
        if (!door->isOpen()) continue;

        sf::FloatRect doorRect = door->getRect();
        if (m_hotPlayer && !m_hotPlayer->isDead() && m_hotPlayer->getRect().findIntersection(doorRect)) {
            if (dynamic_cast<FireDoor*>(door)) { hotAtDoor = true; std::cout << "✓ Hot at Fire Door\n"; }
        }
        if (m_coldPlayer && !m_coldPlayer->isDead() && m_coldPlayer->getRect().findIntersection(doorRect)) {
            if (dynamic_cast<WaterDoor*>(door)) { coldAtDoor = true; std::cout << "✓ Cold at Water Door\n"; }
        }
    }

    if (hotAtDoor && coldAtDoor) { std::cout << ">>> WIN CONDITION MET <<<\n"; return true; }
    return false;
}

void Game::cleanup() {
    for (auto* player : m_players) delete player;
    for (auto* door : m_doors) delete door;
    for (auto* gate : m_gates) delete gate;

    m_players.clear();
    m_doors.clear();
    m_gates.clear();

    m_hotPlayer = nullptr;
    m_coldPlayer = nullptr;
}
