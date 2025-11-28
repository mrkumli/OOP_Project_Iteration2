#include "include/Board.h"
#include <fstream>
#include <sstream>
#include <iostream>

Board::Board(const std::string& path) {
    loadMap(path);
    loadImages();
    generateCollidables();
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
            row.push_back(tile);
        }

        m_gameMap.push_back(row);
    }

    file.close();
    std::cout << "Loaded map with " << m_gameMap.size() << " rows" << std::endl;
}

void Board::loadImages() {
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("data/board_textures/wall.png")) {
        std::cerr << "Failed to load background texture" << std::endl;
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
        if (!texture.loadFromFile(path)) {
            std::cerr << "Texture Load Failed on: " << path << std::endl;
        }
        m_textures[name] = texture;
    }

    std::cout << "Loaded " << m_textures.size() << " textures" << std::endl;
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

            if (tile != "0" && tile != "2" && tile != "3" && tile != "4") {
                m_solidBlocks.emplace_back(
                    sf::Vector2f(xPos, yPos),
                    sf::Vector2f(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE))
                );
            }

            if (tile == "2") {
                m_lavaPools.emplace_back(
                    sf::Vector2f(xPos, yPos + CHUNK_SIZE / 2.0f),
                    sf::Vector2f(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE / 2.0f))
                );
            }

            if (tile == "3") {
                m_waterPools.emplace_back(
                    sf::Vector2f(xPos, yPos + CHUNK_SIZE / 2.0f),
                    sf::Vector2f(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE / 2.0f))
                );
            }

            if (tile == "4") {
                m_gooPools.emplace_back(
                    sf::Vector2f(xPos, yPos + CHUNK_SIZE / 2.0f),
                    sf::Vector2f(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE / 2.0f))
                );
            }
        }
    }

    std::cout << "Generated collidables:" << std::endl;
    std::cout << "  Solid blocks: " << m_solidBlocks.size() << std::endl;
    std::cout << "  Lava pools: " << m_lavaPools.size() << std::endl;
    std::cout << "  Water pools: " << m_waterPools.size() << std::endl;
    std::cout << "  Goo pools: " << m_gooPools.size() << std::endl;
}

const std::vector<sf::FloatRect>& Board::getSolidBlocks() const {
    return m_solidBlocks;
}

const std::vector<sf::FloatRect>& Board::getLavaPools() const {
    return m_lavaPools;
}

const std::vector<sf::FloatRect>& Board::getWaterPools() const {
    return m_waterPools;
}

const std::vector<sf::FloatRect>& Board::getGooPools() const {
    return m_gooPools;
}

const std::map<std::string, sf::Texture>& Board::getTextures() const {
    return m_textures;
}

const std::vector<std::vector<std::string>>& Board::getGameMap() const {
    return m_gameMap;
}
