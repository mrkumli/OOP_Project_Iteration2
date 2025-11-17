#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <optional>

class LevelSelect {
private:
    sf::Texture m_background;
    std::map<int, sf::Texture> m_levelTextures;
    std::optional<sf::Sprite> m_backgroundSprite;
    std::map<int, sf::Sprite> m_levelSprites;

    int m_selectedLevel;

public:
    LevelSelect();

    void draw(sf::RenderWindow& window);
    std::string getSelectedLevel(const sf::Event& event);

private:
    void loadImages();
};

#endif // LEVELSELECT_H
