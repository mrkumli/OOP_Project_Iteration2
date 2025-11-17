#include "include/LevelSelect.h"
#include <iostream>

LevelSelect::LevelSelect()
    : m_selectedLevel(0)
{
    loadImages();
}

void LevelSelect::loadImages() {
    if (!m_background.loadFromFile("data/screens/level_select_screen.png")) {
        std::cerr << "Warning: Failed to load level select background" << std::endl;
    }
    m_backgroundSprite.emplace(m_background);

    for (int i = 1; i <= 5; ++i) {
        sf::Texture texture;
        std::string path = "data/screens/level" + std::to_string(i) + ".png";
        if (!texture.loadFromFile(path)) {
            std::cerr << "Warning: Failed to load level texture: " << path << std::endl;
        }
        m_levelTextures[i] = texture;

        m_levelSprites[i] = sf::Sprite(m_levelTextures[i]);
    }
}

void LevelSelect::draw(sf::RenderWindow& window) {
    if (m_backgroundSprite) window.draw(*m_backgroundSprite);

    float startY = 100.0f;
    float spacing = 50.0f;

    for (int i = 1; i <= 5; ++i) {
        sf::Sprite& sprite = m_levelSprites[i];

        float x = (window.getSize().x - sprite.getGlobalBounds().size.x) / 2.0f;
        float y = startY + (i - 1) * spacing;

        sprite.setPosition(sf::Vector2f(x, y));
        window.draw(sprite);
    }
}

std::string LevelSelect::getSelectedLevel(const sf::Event& event) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Down:
                m_selectedLevel = (m_selectedLevel + 1) % 5;
                break;
            case sf::Keyboard::Key::Up:
                m_selectedLevel = (m_selectedLevel - 1 + 5) % 5;
                break;
            case sf::Keyboard::Key::Enter:
                switch (m_selectedLevel) {
                    case 0: return "level1";
                    case 1: return "level2";
                    case 2: return "level3";
                    case 3: return "level4";
                    case 4: return "level5";
                    default: return "level1";
                }
            default:
                break;
        }
    }

    return "";
}
