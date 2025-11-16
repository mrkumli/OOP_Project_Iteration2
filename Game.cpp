#include<Game.h>
#include<iostream>


Game::Game():
    m_window(sf::VideoMode({640, 480}), "Hot and Cold"),
    m_board("data/level1.txt")
{
    m_window.setFramerateLimit(60);
}
