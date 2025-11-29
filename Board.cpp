#include "include/Board.h"
#include <fstream>
#include <iostream>
#include <algorithm> // for std::remove

Board::Board(const std::string& path) {
    loadMap(path);
    loadImages();
    generateCollidables();
}

void Board::loadMap(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "CRITICAL FAILED: Could not open map file: " << path << std::endl;
        // Load a dummy map so the game doesn't crash
        m_gameMap.resize(30, std::vector<std::string>(40, "0"));
        return;
    }

    std::cout << "Loading map: " << path << std::endl;
    std::string line;

    while (std::getline(file, line)) {
        // Remove carriage returns (Windows CRLF fix)
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

        if (line.empty()) continue;

        std::vector<std::string> row;
        std::string cell;

        // Manual parsing of comma-separated values
        for (char c : line) {
            if (c == ',') {
                row.push_back(cell);
                cell.clear();
            } else {
                cell += c;
            }
        }
        // Push the last cell
        row.push_back(cell);

        // 🔧 Normalize row length to exactly 40 columns
        if (row.size() < 40) {
            row.resize(40, "0");   // pad with empty tiles
        } else if (row.size() > 40) {
            row.resize(40);        // truncate extras
        }

        m_gameMap.push_back(row);
    }

    file.close();

    std::cout << "Map loaded successfully. Rows: " << m_gameMap.size();
    if (!m_gameMap.empty()) {
        std::cout << ", Columns in first row: " << m_gameMap[0].size();
    }
    std::cout << std::endl;
}

void Board::loadImages() {
    // Background texture
    sf::Texture backgroundTexture;
    if (backgroundTexture.loadFromFile("data/board_textures/wall.png")) {
        m_textures["background"] = backgroundTexture;
    } else {
        std::cerr << "Warning: background texture not found." << std::endl;
    }

    // Tile textures
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
            std::cerr << "Warning: texture not found for tile " << name << std::endl;
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

            sf::Vector2f pos(static_cast<float>(x * CHUNK_SIZE), static_cast<float>(y * CHUNK_SIZE));
            sf::Vector2f fullSize(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE));
            sf::Vector2f halfSize(static_cast<float>(CHUNK_SIZE), static_cast<float>(CHUNK_SIZE / 2.0f));
            sf::Vector2f lowerHalfPos(pos.x, pos.y + CHUNK_SIZE / 2.0f);

            // Solid blocks: everything except empty (0) and hazards (2,3,4)
            if (tile != "0" && tile != "2" && tile != "3" && tile != "4" && !tile.empty()) {
                m_solidBlocks.emplace_back(pos, fullSize);
            }

            // Hazards: use lower half of tile
            if (tile == "2") m_lavaPools.emplace_back(lowerHalfPos, halfSize);
            if (tile == "3") m_waterPools.emplace_back(lowerHalfPos, halfSize);
            if (tile == "4") m_gooPools.emplace_back(lowerHalfPos, halfSize);
        }
    }

    std::cout << "Generated " << m_solidBlocks.size() << " solid blocks." << std::endl;
}

const std::vector<sf::FloatRect>& Board::getSolidBlocks() const { return m_solidBlocks; }
const std::vector<sf::FloatRect>& Board::getLavaPools() const { return m_lavaPools; }
const std::vector<sf::FloatRect>& Board::getWaterPools() const { return m_waterPools; }
const std::vector<sf::FloatRect>& Board::getGooPools() const { return m_gooPools; }
const std::map<std::string, sf::Texture>& Board::getTextures() const { return m_textures; }
const std::vector<std::vector<std::string>>& Board::getGameMap() const { return m_gameMap; }
