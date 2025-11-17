// Gates.cpp
#include "Gates.h"
#include <iostream>

Gates::Gates(const sf::Vector2f& gateLocation, const std::vector<sf::Vector2f>& plateLocations)
    : m_gateTexture(),
      m_plateTexture(),
      m_gateLocation(gateLocation),
      m_plateLocations(plateLocations),
      m_isPressed(false),
      m_isOpen(false)
{
    loadImages();

    m_gateRect = sf::FloatRect(
        m_gateLocation,
        sf::Vector2f(static_cast<float>(m_gateTexture.getSize().x),
                    static_cast<float>(m_gateTexture.getSize().y))
    );

    for (const auto& location : m_plateLocations) {
        m_plateRects.emplace_back(
            location,
            sf::Vector2f(static_cast<float>(m_plateTexture.getSize().x),
                        static_cast<float>(m_plateTexture.getSize().y))
        );

        sf::Sprite plateSprite(m_plateTexture);
        plateSprite.setPosition(location);
        m_plateSprites.push_back(plateSprite);
    }

    m_gateSprite.setTexture(m_gateTexture);
    m_gateSprite.setPosition(m_gateLocation);
}

void Gates::loadImages() {
    if (!m_gateTexture.loadFromFile("data/gates_and_plates/gate.png")) {
        std::cerr << "Warning: Failed to load gate texture" << std::endl;
    }

    if (!m_plateTexture.loadFromFile("data/gates_and_plates/plate.png")) {
        std::cerr << "Warning: Failed to load plate texture" << std::endl;
    }
}

void Gates::tryOpen(const std::list<Character*>& players) {
    bool platePressed = false;

    for (const auto* player : players) {
        if (!player || player->isDead()) continue;

        for (const auto& plateRect : m_plateRects) {
            if (checkCollision(player->getRect(), plateRect)) {
                platePressed = true;
                break;
            }
        }

        if (platePressed) break;
    }

    m_isPressed = platePressed;

    if (m_isPressed && !m_isOpen) {
        m_gateLocation.y -= 2 * CHUNK_SIZE;
        m_gateRect.position.y -= 2 * CHUNK_SIZE;
        m_gateSprite.setPosition(m_gateLocation);
        m_isOpen = true;
    }
    else if (!m_isPressed && m_isOpen) {
        m_gateLocation.y += 2 * CHUNK_SIZE;
        m_gateRect.position.y += 2 * CHUNK_SIZE;
        m_gateSprite.setPosition(m_gateLocation);
        m_isOpen = false;
    }
}

void Gates::draw(sf::RenderWindow& window) {
    window.draw(m_gateSprite);

    for (const auto& plateSprite : m_plateSprites) {
        window.draw(plateSprite);
    }
}

sf::FloatRect Gates::getGateRect() const {
    return m_gateRect;
}

const std::vector<sf::FloatRect>& Gates::getPlateRects() const {
    return m_plateRects;
}

bool Gates::checkCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) const {
    return rect1.findIntersection(rect2).has_value();
}
