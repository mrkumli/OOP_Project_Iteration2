#include <SFML/Graphics.hpp>

int main() {
    // sf::RenderWindow window(sf::VideoMode(sf::Vector2u(800, 600)), "Basic SFML Window");
    // ## Arguments: (Video Mode, Title, Style, State)
    sf::Window window(sf::VideoMode({800, 600}), "My window", sf::Style::Default, sf::State::Windowed);
    while(window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }
            }
    }
    return 0;
}
