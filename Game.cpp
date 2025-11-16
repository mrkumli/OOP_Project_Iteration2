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

        // if (event->is<sf::Event::KeyPressed>()){
        //     if(event->key.code == sf::Keyboard::Escape){
        //         m_window.close();
        //     }
        // } Not necessary since it will just exit the entire program everytime theres escape
    }
}

void Game::update()
{
    for (auto* player:m_players){
        if(player && !player->isDead()){
            player->update(m_board);
        }
    }

    checkCollisions();
    checkDeath();

    for(auto* player:m_players){
        if(gate){
            gate->tryOpen(m_players);
        }
    }

    if(checkWin()){
        std::cout<< "Level Completed!" << std::endl;
    }
}


void Game::draw()
{
    //Gats
    for (auto* player:m_players){
        if(gate){
            gate->draw(m_window);
        }
    }
    //Doors
    for(auto* player:m_players){
        if(door){
            door->draw(m_window);
        }
    }
    //Players
    for(auto* players:m_players){
        if(player && !player->isDead()){
            player->draw(m_window);
        }
    }

    m_window.display();//displaying everything
}


void Game::checkCollisions()
{
    for(auto* player:m_players){
        if(!door){//check through all the doors if collision is true with either
            continue;
        }
    }

    for(auto* player:m_players){
        if(!player || player->isDead()){
            continue;
        }
        door->tryOpen(*player);
    }
}


void Game::checkDeath()
{
    for(auto* player:m_players){
        if(!player || player->isDead()){
            continue;
        }
    }

    sf::FloatRect playerRect = player->getGlobalBounds();
    sf::FloatRect playerType = player->getType();

    if(playerType == "cold"){
        for(const auto* lavaPool:m_board.getLavaPools()){
            if(playerRect.findIntersection(lavaPool)){
                player->kill();
                std::cout << "Cold died in Lava" << std::endl;
                break;
            }

        }
    }

    if(playerType == "hot"){
        for(const auto* waterPool:m_board.getWaterPools()){
            if (playerRect.findIntersection(waterPool)) {
                player->kill();
                std::cout << "Hot died in Water" << std::endl;
                break;
            }
        }
    }

    for(const auto* gooPool:m_board.getGooPools()){
        if(playerRect.findIntersection(gooPool)){
            player->kill();
            std::cout<<"Player died in Goo Pool" << std::endl;
            break;
        }
    }

}

void Game::checkWin() {
    for(auto* door:m_doors){
        if(door && !door->isOpen()) {
            return false;
        }

    }
    return true;
}
