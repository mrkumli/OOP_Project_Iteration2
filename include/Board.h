#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

class Board {
private:
    std::vector<std::vector<std::string>> m_gameMap;
    std::map<std::string, sf::Texture> m_textures;
    std::vector<sf::FloatRect> m_solidBlocks;
    std::vector<sf::FloatRect> m_lavaPools;
    std::vector<sf::FloatRect> m_waterPools;
    std::vector<sf::FloatRect> m_gooPools;

    static constexpr int CHUNK_SIZE = 16;

public:
    Board(const std::string& path);

    void loadMap(const std::string& path);
    void loadImages();
    void generateCollidables();

    const std::vector<sf::FloatRect>& getSolidBlocks() const;
    const std::vector<sf::FloatRect>& getLavaPools() const;
    const std::vector<sf::FloatRect>& getWaterPools() const;
    const std::vector<sf::FloatRect>& getGooPools() const;
    const std::map<std::string, sf::Texture>& getTextures() const;
    const std::vector<std::vector<std::string>>& getGameMap() const;
};

#endif // BOARD_H
