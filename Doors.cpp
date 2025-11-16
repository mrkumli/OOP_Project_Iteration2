#include "Doors.h"
#include<iostream>


Doors::Doors(const sf::Vector2f& position):
    m_doorLocation(doorLocation),
    m_isOpen(false),
    m_heightRaised(0.0f),
    m_playerAtDoor(false)
{
    loadCommonImages();
}

void Doors::loadCommonImages() {
    if (!m_frameTexture.loadFromFile("data/door_images/door_frame.png")) {
        std::cerr << "Door Frame Picture Error" << std::endl;
    }
    if (!m_backgroundTexture.loadFromFile("data/door_images/door_background.png")) {
        std::cerr << "Door Background Picture Error" << std::endl;
    }

    m_frameSprite.setTexture(m_frameTexture);
    m_backgroundSprite.setTexture(m_backgroundTexture);
    //Sprites Positioning on the screen
    m_backgroundSprite.setPosition(m_doorLocation);
    m_frameSprite.setPosition(m_doorLocation.x - CHUNK_SIZE, m_doorLocation.y - 2 * CHUNK_SIZE);
}

void Doors::tryRaiseDoor() {
    //Open door when player is at door
    if (m_playerAtDoor && !m_isOpen) {
        m_doorLocation.y -= DOOR_SPEED;
        m_heightRaised += DOOR_SPEED;

        if (m_heightRaised >= 31.0f) {
            m_isOpen = true;
        }

        m_doorSprite.setPosition(m_doorLocation);
    }
    //Door down when the player leaves
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
