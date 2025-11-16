#include<Game.h>
#include<iostream>


Game::Game():
    m_window(sf::VideoMode({640, 480}), "Hot and Cold"),
    m_board("data/level1.txt")
{
    m_window.setFramerateLimit(60);
}

Game::~Game()
{
   cleanup();
}

void Game::run()
{
    while (m_window.isOpen())
    {
        handleEvents();
        update();
        draw();
    }
}

void Game::handleEvents()
{
    while(const auto event == m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>())
            m_window.close();
    }
}
