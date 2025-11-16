#include "Game.h"
#include "Character.h"
#include "Doors.h"
#include "Gates.h"
#include <iostream>

int main() {
    try {
        Game game;

        Hot* hotPlayer = new Hot(sf::Vector2f(16.0f, 336.0f));
        Cold* coldPlayer = new Cold(sf::Vector2f(35.0f, 336.0f));


        FireDoor* fireDoor = new FireDoor(sf::Vector2f(64.0f, 48.0f));
        WaterDoor* waterDoor = new WaterDoor(sf::Vector2f(128.0f, 48.0f));
        std::vector<sf::Vector2f> plateLocations = {
            sf::Vector2f(190.0f, 168.0f),
            sf::Vector2f(390.0f, 168.0f)
        };
        Gates* gate = new Gates(sf::Vector2f(285.0f, 128.0f), plateLocations);
        game.run();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
