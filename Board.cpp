#include "include/Board.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

Board::Board(const std::string& path) {
    loadMap(path);
    loadImages();
    generateCollidables();
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

void Board::loadMap(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string tile;

        while (std::getline(ss, tile, ',')) {
            row.push_back(trim(tile));
        }

        if (!row.empty()) {
            m_gameMap.push_back(row);
        }
    }
    file.close();
    std::cout << "Loaded map with " << m_gameMap.size() << " rows." << std::endl;
}

void Board::loadImages() {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("data/board_textures/wall.png")) {
        // Fallback for background
        sf::Image fallback(sf::Vector2u(16, 16), sf::Color(100, 100, 100));
        if (!backgroundTexture.loadFromImage(fallback)) {
            std::cerr << "Critical: Failed to load fallback background." << std::endl;
        }
    }
    m_textures["background"] = backgroundTexture;

    std::vector<std::string> textureNames = {
        "100", "111", "112", "113", "114",
        "121", "122", "123", "124",
        "2", "3", "4"
    };

    for (const auto& name : textureNames) {
        sf::Texture texture;
        std::string path = "data/board_textures/" + name + ".png";

        if (texture.loadFromFile(path)) {
            m_textures[name] = texture;
        } else {
            // SFML 3 FIX: Use Vector2u constructor, then resize if reusing
            sf::Image fallback(sf::Vector2u(16, 16), sf::Color::Magenta);

            if (name == "2") fallback.resize(sf::Vector2u(16, 16), sf::Color(255, 100, 0)); // Lava
            else if (name == "3") fallback.resize(sf::Vector2u(16, 16), sf::Color(0, 100, 255)); // Water
            else if (name == "4") fallback.resize(sf::Vector2u(16, 16), sf::Color(0, 255, 0)); // Goo
            else fallback.resize(sf::Vector2u(16, 16), sf::Color(80, 80, 80)); // Wall

            if (texture.loadFromImage(fallback)) {
                m_textures[name] = texture;
            }
        }
    }
}

void Board::generateCollidables() {
    m_solidBlocks.clear();
    m_lavaPools.clear();
    m_waterPools.clear();
    m_gooPools.clear();

    for (size_t y = 0; y < m_gameMap.size(); ++y) {
        for (size_t x = 0; x < m_gameMap[y].size(); ++x) {
            const std::string& tile = m_gameMap[y][x];

            float xPos = static_cast<float>(x * CHUNK_SIZE);
            float yPos = static_cast<float>(y * CHUNK_SIZE);

            sf::Vector2f pos(xPos, yPos);
            sf::Vector2f fullSize(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE));
            sf::Vector2f halfSize(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE / 2.0f));
            sf::Vector2f halfPos(xPos, yPos + CHUNK_SIZE / 2.0f);

            if (tile != "0" && tile != "2" && tile != "3" && tile != "4") {
                m_solidBlocks.emplace_back(pos, fullSize);
            }

            if (tile == "2") m_lavaPools.emplace_back(halfPos, halfSize);
            if (tile == "3") m_waterPools.emplace_back(halfPos, halfSize);
            if (tile == "4") m_gooPools.emplace_back(halfPos, halfSize);
        }
    }
}

const std::vector<sf::FloatRect>& Board::getSolidBlocks() const { return m_solidBlocks; }
const std::vector<sf::FloatRect>& Board::getLavaPools() const { return m_lavaPools; }
const std::vector<sf::FloatRect>& Board::getWaterPools() const { return m_waterPools; }
const std::vector<sf::FloatRect>& Board::getGooPools() const { return m_gooPools; }
const std::map<std::string, sf::Texture>& Board::getTextures() const { return m_textures; }
const std::vector<std::vector<std::string>>& Board::getGameMap() const { return m_gameMap; }
