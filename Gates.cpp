#include "include/Gates.h"
#include <iostream>

Gates::Gates(const sf::Vector2f& gateLocation, const std::vector<sf::Vector2f>& plateLocations)
    : m_gateLocation(gateLocation),
      m_plateLocations(plateLocations),
      m_isPressed(false),
      m_isOpen(false)
{
    loadImages();

    m_gateRect = sf::FloatRect(
        m_gateLocation,
        sf::Vector2f(32.0f, 96.0f) // Hardcoded approximate size if texture fails
    );

    // RELOAD LOGIC TO BE SAFE:
    if (m_gateTexture.getSize().x > 0) {
         m_gateRect.size = sf::Vector2f(static_cast<float>(m_gateTexture.getSize().x),
                                        static_cast<float>(m_gateTexture.getSize().y));
    }

    for (const auto& location : m_plateLocations) {
        sf::Vector2f size(32.0f, 16.0f);
        if (m_plateTexture.getSize().x > 0) {
            size = sf::Vector2f(static_cast<float>(m_plateTexture.getSize().x),
                                static_cast<float>(m_plateTexture.getSize().y));
        }

        m_plateRects.emplace_back(location, size);
        sf::Sprite plateSprite(m_plateTexture);
        plateSprite.setPosition(location);
        m_plateSprites.push_back(plateSprite);
    }
    m_gateSprite.emplace(m_gateTexture);
    m_gateSprite->setPosition(m_gateLocation);
}

void Gates::loadImages() {
    if (!m_gateTexture.loadFromFile("data/gates_and_plates/gate.png")) {}
    if (!m_plateTexture.loadFromFile("data/gates_and_plates/plate.png")) {}
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
        if (m_gateSprite) m_gateSprite->setPosition(m_gateLocation);
        m_isOpen = true;
    }
    else if (!m_isPressed && m_isOpen) {
        m_gateLocation.y += 2 * CHUNK_SIZE;
        m_gateRect.position.y += 2 * CHUNK_SIZE;
        if (m_gateSprite) m_gateSprite->setPosition(m_gateLocation);
        m_isOpen = false;
    }
}

void Gates::draw(sf::RenderWindow& window) {
    if (m_gateSprite) window.draw(*m_gateSprite);
    for (const auto& plateSprite : m_plateSprites) window.draw(plateSprite);
}

//Gates Rectangle
sf::FloatRect Gates::getGateRect() const {
    return m_gateRect;
}

//Plates
const std::vector<sf::FloatRect>& Gates::getPlateRects() const {
    return m_plateRects;
}

bool Gates::isOpen() const {
    return m_isOpen;
}

//Cllosion check for both
bool Gates::checkCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) const {
    return rect1.findIntersection(rect2).has_value();
}
