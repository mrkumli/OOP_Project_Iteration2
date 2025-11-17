#include "Doors.h"
#include <iostream>

Doors::Doors(const sf::Vector2f& doorLocation)
    : m_doorLocation(doorLocation),
      m_isOpen(false),
      m_heightRaised(0.0f),
      m_playerAtDoor(false)
{
    loadCommonImages();
}

void Doors::loadCommonImages() {
    if (!m_frameTexture.loadFromFile("data/door_images/door_frame.png")) {
        std::cerr << "Warning: Failed to load door frame texture" << std::endl;
    }
    if (!m_backgroundTexture.loadFromFile("data/door_images/door_background.png")) {
        std::cerr << "Warning: Failed to load door background texture" << std::endl;
    }

    m_frameSprite.setTexture(m_frameTexture);
    m_backgroundSprite.setTexture(m_backgroundTexture);

    m_backgroundSprite.setPosition(m_doorLocation);
    m_frameSprite.setPosition(m_doorLocation.x - CHUNK_SIZE,
                              m_doorLocation.y - 2 * CHUNK_SIZE);
}

void Doors::tryRaiseDoor() {
    if (m_playerAtDoor && !m_isOpen) {
        m_doorLocation.y -= DOOR_SPEED;
        m_heightRaised += DOOR_SPEED;

        if (m_heightRaised >= 31.0f) {
            m_isOpen = true;
        }

        m_doorSprite.setPosition(m_doorLocation);
    }
    else if (!m_playerAtDoor && m_heightRaised > 0.0f) {
        m_doorLocation.y += DOOR_SPEED;
        m_heightRaised -= DOOR_SPEED;
        m_isOpen = false;

        m_doorSprite.setPosition(m_doorLocation);
    }
}

void Doors::draw(sf::RenderWindow& window) {
    window.draw(m_backgroundSprite);
    window.draw(m_doorSprite);
    window.draw(m_frameSprite);
}

bool Doors::isOpen() const {
    return m_isOpen;
}

sf::FloatRect Doors::getRect() const {
    return m_rect;
}

// FireDoor - HOT PLAYER ONLY
FireDoor::FireDoor(const sf::Vector2f& doorLocation)
    : Doors(doorLocation)
{
    if (!m_doorTexture.loadFromFile("data/door_images/fire_door.png")) {
        std::cerr << "Warning: Failed to load fire door texture" << std::endl;
    }
    m_doorSprite.setTexture(m_doorTexture);
    m_doorSprite.setPosition(m_doorLocation);

    m_rect = sf::FloatRect(m_doorLocation,
                          sf::Vector2f(static_cast<float>(m_doorTexture.getSize().x),
                                      static_cast<float>(m_doorTexture.getSize().y)));
}

void FireDoor::tryOpen(Character& player) {
    if (player.getRect().findIntersection(m_rect)) {
        if (player.getType() == "hot") {
            m_playerAtDoor = true;
        } else {
            m_playerAtDoor = false;
        }
    } else {
        m_playerAtDoor = false;
    }

    tryRaiseDoor();
}

// WaterDoor - COLD PLAYER ONLY
WaterDoor::WaterDoor(const sf::Vector2f& doorLocation)
    : Doors(doorLocation)
{
    if (!m_doorTexture.loadFromFile("data/door_images/water_door.png")) {
        std::cerr << "Warning: Failed to load water door texture" << std::endl;
    }
    m_doorSprite.setTexture(m_doorTexture);
    m_doorSprite.setPosition(m_doorLocation);

    m_rect = sf::FloatRect(m_doorLocation,
                          sf::Vector2f(static_cast<float>(m_doorTexture.getSize().x),
                                      static_cast<float>(m_doorTexture.getSize().y)));
}

void WaterDoor::tryOpen(Character& player) {
    if (player.getRect().findIntersection(m_rect)) {
        if (player.getType() == "cold") {
            m_playerAtDoor = true;
        } else {
            m_playerAtDoor = false;
        }
    } else {
        m_playerAtDoor = false;
    }

    tryRaiseDoor();
}
