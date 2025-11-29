#include "include/Game.h"
#include "include/Controller.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({640, 480}), "Hot and Cold - SFML 3"),
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
    // FIX: Adjusted spawn Y to 352.0f (Row 22) to ensure they land on the floor (Row 24)
    m_hotPlayer = new Hot(sf::Vector2f(32.0f, 352.0f));
    m_coldPlayer = new Cold(sf::Vector2f(580.0f, 352.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    // Create doors at TOP of level
    m_doors.push_back(new FireDoor(sf::Vector2f(32.0f, 48.0f)));
    m_doors.push_back(new WaterDoor(sf::Vector2f(576.0f, 48.0f)));

    // Create a gate with pressure plate
    std::vector<sf::Vector2f> platePositions = {
        sf::Vector2f(312.0f, 448.0f)
    };
    m_gates.push_back(new Gates(sf::Vector2f(312.0f, 240.0f), platePositions));
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
                    cleanup();
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
        if (player && !player->isDead()) {
            player->update(m_board);
        }
    }

    checkDeath();

    for (auto* door : m_doors) {
        for (auto* player : m_players) {
            if (player && !player->isDead()) door->tryOpen(*player);
        }
    }

    for (auto* gate : m_gates) gate->tryOpen(m_players);

    checkCollisions();

    if (checkWin()) m_gameState = GameState::Won;

    bool hotDead = (m_hotPlayer == nullptr || m_hotPlayer->isDead());
    bool coldDead = (m_coldPlayer == nullptr || m_coldPlayer->isDead());

    if (hotDead && coldDead && m_gameState == GameState::Playing) {
        m_gameState = GameState::Lost;
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

    // Draw background
    auto bgIt = textures.find("background");
    if (bgIt != textures.end()) {
        sf::Sprite bgSprite(bgIt->second);
        sf::Vector2u windowSize = m_window.getSize();
        sf::FloatRect spriteSize = bgSprite.getLocalBounds();

        // SFML 3: Use .size.x / .size.y
        bgSprite.setScale({
            static_cast<float>(windowSize.x) / spriteSize.size.x,
            static_cast<float>(windowSize.y) / spriteSize.size.y
        });
        m_window.draw(bgSprite);
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
                    tileSprite.setPosition({
                        static_cast<float>(x * CHUNK_SIZE),
                        static_cast<float>(y * CHUNK_SIZE)
                    });
                    m_window.draw(tileSprite);
                }
            }
        }
    }
}

void Game::drawGameStateText() {
    // Simple overlay for Win/Loss
    if (m_gameState == GameState::Won || m_gameState == GameState::Lost) {
        sf::RectangleShape overlay({500.0f, 150.0f});
        overlay.setPosition({70.0f, 165.0f});
        if (m_gameState == GameState::Won) overlay.setFillColor(sf::Color(0, 180, 0, 230));
        else overlay.setFillColor(sf::Color(180, 0, 0, 230));
        m_window.draw(overlay);
    }
}

void Game::checkDeath() {
    for (auto* player : m_players) {
        if (!player || player->isDead()) continue;

        sf::FloatRect playerRect = player->getRect();
        std::string playerType = player->getType();

        if (playerType == "cold") {
            for (const auto& lava : m_board.getLavaPools()) {
                if (playerRect.findIntersection(lava)) { player->kill(); break; }
            }
        }
        if (playerType == "hot") {
            for (const auto& water : m_board.getWaterPools()) {
                if (playerRect.findIntersection(water)) { player->kill(); break; }
            }
        }
        if (!player->isDead()) {
            for (const auto& goo : m_board.getGooPools()) {
                if (playerRect.findIntersection(goo)) { player->kill(); break; }
            }
        }
    }
}

void Game::checkCollisions() {
    for (auto* gate : m_gates) {
        if (gate->isOpen()) continue;
        sf::FloatRect gateRect = gate->getGateRect();
        for (auto* player : m_players) {
            if (!player || player->isDead()) continue;
            // Simple block check
            if (player->getRect().findIntersection(gateRect)) {
                // In a full physics engine, we would push back here
            }
        }
    }
}

bool Game::checkWin() {
    bool hotAtDoor = false;
    bool coldAtDoor = false;

    for (auto* door : m_doors) {
        if (!door->isOpen()) continue;
        sf::FloatRect doorRect = door->getRect();

        if (m_hotPlayer && !m_hotPlayer->isDead()) {
            if (m_hotPlayer->getRect().findIntersection(doorRect)) {
                if (dynamic_cast<FireDoor*>(door)) hotAtDoor = true;
            }
        }
        if (m_coldPlayer && !m_coldPlayer->isDead()) {
            if (m_coldPlayer->getRect().findIntersection(doorRect)) {
                if (dynamic_cast<WaterDoor*>(door)) coldAtDoor = true;
            }
        }
    }
    return hotAtDoor && coldAtDoor;
}

void Game::cleanup() {
    for (auto* p : m_players) delete p;
    m_players.clear();
    for (auto* d : m_doors) delete d;
    m_doors.clear();
    for (auto* g : m_gates) delete g;
    m_gates.clear();
    m_hotPlayer = nullptr;
    m_coldPlayer = nullptr;
}
