#include "include/Doors.h"
#include <iostream>

Doors::Doors(const sf::Vector2f& doorLocation)
    : m_isOpen(false),
      m_heightRaised(0.0f),
      m_playerAtDoor(false),
      m_doorLocation(doorLocation)
{
    loadCommonImages();
}

void Doors::loadCommonImages() {
    if (!m_frameTexture.loadFromFile("data/door_images/door_frame.png")) {}
    if (!m_backgroundTexture.loadFromFile("data/door_images/door_background.png")) {}

    m_frameSprite.emplace(m_frameTexture);
    m_backgroundSprite.emplace(m_backgroundTexture);

    m_backgroundSprite->setPosition(m_doorLocation);
    m_frameSprite->setPosition(sf::Vector2f(
        m_doorLocation.x - CHUNK_SIZE/2,
        m_doorLocation.y - 2 * CHUNK_SIZE/2
    ));
}

void Doors::tryRaiseDoor() {
    if (m_playerAtDoor && !m_isOpen) {
        m_doorLocation.y -= DOOR_SPEED;
        m_heightRaised += DOOR_SPEED;
        if (m_heightRaised >= 31.0f && !m_isOpen) {
            m_isOpen = true;
            std::cout << "[DOOR] Door fully OPEN!" << std::endl;
        }
        if (m_doorSprite) m_doorSprite->setPosition(m_doorLocation);
    }
    else if (!m_playerAtDoor && m_heightRaised > 0.0f) {
        m_doorLocation.y += DOOR_SPEED;
        m_heightRaised -= DOOR_SPEED;
        if (m_heightRaised <= 0.0f && m_isOpen) {
            m_isOpen = false;
            std::cout << "[DOOR] Door CLOSED" << std::endl;
        }
        if (m_doorSprite) m_doorSprite->setPosition(m_doorLocation);
    }
}

void Doors::draw(sf::RenderWindow& window) {
    if (m_backgroundSprite) window.draw(*m_backgroundSprite);
    if (m_doorSprite) window.draw(*m_doorSprite);
    if (m_frameSprite) window.draw(*m_frameSprite);
}

bool Doors::isOpen() const { return m_isOpen; }
sf::FloatRect Doors::getRect() const { return m_rect; }

FireDoor::FireDoor(const sf::Vector2f& doorLocation) : Doors(doorLocation) {
    if (!m_doorTexture.loadFromFile("data/door_images/fire_door.png")) {}
    m_doorSprite.emplace(m_doorTexture);
    m_doorSprite->setPosition(m_doorLocation);
    m_rect = sf::FloatRect(m_doorLocation, sf::Vector2f(static_cast<float>(m_doorTexture.getSize().x), static_cast<float>(m_doorTexture.getSize().y)));

    std::cout << "FireDoor created at position: " << doorLocation.x << ", " << doorLocation.y << std::endl;
}

void FireDoor::tryOpen(Character& player) {
    sf::FloatRect detectionZone = m_rect;
    detectionZone.position.x -= 10.0f;
    detectionZone.position.y -= 10.0f;
    detectionZone.size.x += 20.0f;
    detectionZone.size.y += 20.0f;

    bool wasAtDoor = m_playerAtDoor;

    if (player.getRect().findIntersection(detectionZone)) {
        if (player.getType() == "hot") {
            m_playerAtDoor = true;
            if (!wasAtDoor) {
                std::cout << "[FIRE DOOR] Hot player approaching door..." << std::endl;
            }
        } else {
            m_playerAtDoor = false;
        }
    } else {
        m_playerAtDoor = false;
    }
    tryRaiseDoor();
}

WaterDoor::WaterDoor(const sf::Vector2f& doorLocation) : Doors(doorLocation) {
    if (!m_doorTexture.loadFromFile("data/door_images/water_door.png")) {}
    m_doorSprite.emplace(m_doorTexture);
    m_doorSprite->setPosition(m_doorLocation);
    m_rect = sf::FloatRect(m_doorLocation, sf::Vector2f(static_cast<float>(m_doorTexture.getSize().x), static_cast<float>(m_doorTexture.getSize().y)));

    std::cout << "WaterDoor created at position: " << doorLocation.x << ", " << doorLocation.y << std::endl;
}

void WaterDoor::tryOpen(Character& player) {
    sf::FloatRect detectionZone = m_rect;
    detectionZone.position.x -= 10.0f;
    detectionZone.position.y -= 10.0f;
    detectionZone.size.x += 20.0f;
    detectionZone.size.y += 20.0f;

    bool wasAtDoor = m_playerAtDoor;

    if (player.getRect().findIntersection(detectionZone)) {
        if (player.getType() == "cold") {
            m_playerAtDoor = true;
            if (!wasAtDoor) {
                std::cout << "[WATER DOOR] Cold player approaching door..." << std::endl;
            }
        } else {
            m_playerAtDoor = false;
        }
    } else {
        m_playerAtDoor = false;
    }
    tryRaiseDoor();
}
