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
