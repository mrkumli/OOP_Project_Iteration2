#include "include/Game.h"
#include "include/Controller.h"
#include <iostream>

Game::Game()
    : m_window(sf::VideoMode({640, 480}), "Hot and Cold - Phase 1"),
      m_board("data/level1.txt"),
      m_hotPlayer(nullptr),
      m_coldPlayer(nullptr)
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
    m_hotPlayer = new Hot(sf::Vector2f(16.0f, 336.0f));
    m_coldPlayer = new Cold(sf::Vector2f(35.0f, 336.0f));

    m_players.push_back(m_hotPlayer);
    m_players.push_back(m_coldPlayer);

    std::cout << "Level 1 initialized - Phase 1" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Hot (Red): Arrow Keys" << std::endl;
    std::cout << "  Cold (Blue): WASD" << std::endl;
    std::cout << "  ESC: Quit" << std::endl;
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
        }

        if (m_hotPlayer && !m_hotPlayer->isDead()) {
            m_arrowsController->controlPlayer(*event, *m_hotPlayer);
        }

        if (m_coldPlayer && !m_coldPlayer->isDead()) {
            m_wasdController->controlPlayer(*event, *m_coldPlayer);
        }
    }
}

void Game::update() {
    for (auto* player : m_players) {
        if (player && !player->isDead()) {
            player->update(m_board);
        }
    }

    checkDeath();

    bool allDead = true;
    for (auto* player : m_players) {
        if (player && !player->isDead()) {
            allDead = false;
            break;
        }
    }

    if (allDead) {
        std::cout << "\n=== GAME OVER ===" << std::endl;
        std::cout << "Both players died!" << std::endl;
        sf::sleep(sf::seconds(2));
        m_window.close();
    }
}

void Game::draw() {
    m_window.clear(sf::Color::Black);

    drawBoard();

    for (auto* player : m_players) {
        if (player && !player->isDead()) {
            player->draw(m_window);
        }
    }

    m_window.display();
}

void Game::drawBoard() {
    const auto& textures = m_board.getTextures();
    const auto& gameMap = m_board.getGameMap();

    auto bgIt = textures.find("background");
    if (bgIt != textures.end()) {
        sf::Sprite bgSprite(bgIt->second);
        m_window.draw(bgSprite);
    }

    const int CHUNK_SIZE = 16;
    for (size_t y = 0; y < gameMap.size(); ++y) {
        for (size_t x = 0; x < gameMap[y].size(); ++x) {
            const std::string& tile = gameMap[y][x];

            if (tile != "0") {
                auto it = textures.find(tile);
                if (it != textures.end()) {
                    sf::Sprite tileSprite(it->second);
                    tileSprite.setPosition(static_cast<float>(x * CHUNK_SIZE),
                                          static_cast<float>(y * CHUNK_SIZE));
                    m_window.draw(tileSprite);
                }
            }
        }
    }
}

void Game::checkDeath() {
    for (auto* player : m_players) {
        if (!player || player->isDead()) continue;

        sf::FloatRect playerRect = player->getRect();
        std::string playerType = player->getType();

        // COLD DIES IN LAVA
        if (playerType == "cold") {
            for (const auto& lavaPool : m_board.getLavaPools()) {
                if (playerRect.findIntersection(lavaPool)) {
                    player->kill();
                    std::cout << "\n💀 COLD PLAYER DIED IN LAVA! 💀" << std::endl;
                    break;
                }
            }
        }

        // HOT DIES IN WATER
        if (playerType == "hot") {
            for (const auto& waterPool : m_board.getWaterPools()) {
                if (playerRect.findIntersection(waterPool)) {
                    player->kill();
                    std::cout << "\n💀 HOT PLAYER DIED IN WATER! 💀" << std::endl;
                    break;
                }
            }
        }

        // BOTH DIE IN GOO
        for (const auto& gooPool : m_board.getGooPools()) {
            if (playerRect.findIntersection(gooPool)) {
                player->kill();
                std::cout << "\n💀 PLAYER DIED IN GOO! 💀" << std::endl;
                break;
            }
        }
    }
}

void Game::checkCollisions() {
    // Phase 2
}

bool Game::checkWin() {
    // Phase 2
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
