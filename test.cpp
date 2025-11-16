#include <SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include<iostream>

int main() {
    // sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Basic SFML Window");
    // ## Arguments: (Video Mode, Title, Style, State)
    sf::Window window(sf::VideoMode({800, 600}), "Default Window", sf::Style::Default, sf::State::Windowed);
    window.setVerticalSyncEnabled(true);
    //window.setFramerateLimit(60);
    while(window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {

                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }


                if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                    if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                        window.close();
                    }
                }
                // To check new resized screen res:

                // if (const auto* resized = event->getIf<sf::Event::Resized>())
                // {
                //     std::cout << "new width: " << resized->size.x << std::endl;
                //     std::cout << "new height: " << resized->size.y << std::endl;
                // }
            }
    }
    return 0;
}
