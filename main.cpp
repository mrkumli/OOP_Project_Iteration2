#include "include/Game.h"
#include "include/LevelSelect.h"
#include <iostream>
#include <SFML/Graphics.hpp>

enum class MenuState {
    MainMenu,
    LevelSelect,
    InGame
};

void drawMainMenu(sf::RenderWindow& window, sf::Font& font, int selectedOption) {
    window.clear(sf::Color(20, 20, 40));

    // Title
    sf::Text title(font, "HOT AND COLD", 48);
    title.setFillColor(sf::Color::Yellow);
    title.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = title.getLocalBounds();
    title.setPosition(sf::Vector2f((640 - titleBounds.size.x) / 2, 100));
    window.draw(title);

    // Menu options
    std::vector<std::string> options = {"Start Game", "Level Select", "Quit"};
    float startY = 250;
    float spacing = 60;

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text optionText(font, options[i], 30);

        if (static_cast<int>(i) == selectedOption) {
            optionText.setFillColor(sf::Color::White);
            optionText.setStyle(sf::Text::Bold);

            // Draw selection indicator
            sf::Text arrow(font, ">", 30);
            arrow.setFillColor(sf::Color::White);
            arrow.setPosition(sf::Vector2f(150, startY + i * spacing));
            window.draw(arrow);
        } else {
            optionText.setFillColor(sf::Color(180, 180, 180));
        }

        sf::FloatRect bounds = optionText.getLocalBounds();
        optionText.setPosition(sf::Vector2f((640 - bounds.size.x) / 2, startY + i * spacing));
        window.draw(optionText);
    }

    // Instructions
    sf::Text instructions(font, "Use UP/DOWN to navigate, ENTER to select", 16);
    instructions.setFillColor(sf::Color(150, 150, 150));
    sf::FloatRect instrBounds = instructions.getLocalBounds();
    instructions.setPosition(sf::Vector2f((640 - instrBounds.size.x) / 2, 430));
    window.draw(instructions);

    window.display();
}

int main() {
    try {
        std::cout << "==================================" << std::endl;
        std::cout << "  HOT AND COLD" << std::endl;
        std::cout << "  Co-op Puzzle Platformer" << std::endl;
        std::cout << "==================================" << std::endl;

        sf::RenderWindow window(sf::VideoMode({640, 480}), "Hot and Cold");
        window.setFramerateLimit(60);

        sf::Font font;
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Warning: Could not load font" << std::endl;
        }

        MenuState menuState = MenuState::MainMenu;
        int selectedOption = 0;
        LevelSelect levelSelect;
        Game* game = nullptr;

        while (window.isOpen()) {
            while (const auto event = window.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    window.close();
                }

                if (menuState == MenuState::MainMenu) {
                    if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                        switch (keyPressed->code) {
                            case sf::Keyboard::Key::Up:
                                selectedOption = (selectedOption - 1 + 3) % 3;
                                break;
                            case sf::Keyboard::Key::Down:
                                selectedOption = (selectedOption + 1) % 3;
                                break;
                            case sf::Keyboard::Key::Enter:
                                if (selectedOption == 0) {
                                    // Start Game (Level 1)
                                    std::cout << "\nStarting Level 1..." << std::endl;
                                    menuState = MenuState::InGame;
                                    if (game) delete game;
                                    game = new Game();
                                } else if (selectedOption == 1) {
                                    // Level Select (not implemented yet)
                                    std::cout << "\nLevel Select not yet implemented. Starting Level 1..." << std::endl;
                                    menuState = MenuState::InGame;
                                    if (game) delete game;
                                    game = new Game();
                                } else if (selectedOption == 2) {
                                    // Quit
                                    window.close();
                                }
                                break;
                            case sf::Keyboard::Key::Escape:
                                window.close();
                                break;
                            default:
                                break;
                        }
                    }
                }
            }

            if (menuState == MenuState::MainMenu) {
                drawMainMenu(window, font, selectedOption);
            } else if (menuState == MenuState::InGame) {
                if (game) {
                    // Check if window is still open before running game loop
                    if (window.isOpen()) {
                        // Close the menu window
                        window.close();

                        // Game has its own window and loop
                        game->run();

                        // After game ends, clean up
                        delete game;
                        game = nullptr;

                        // Game ended, exit program
                        return 0;
                    }
                }
            }
        }

        if (game) delete game;

        std::cout << "\nGame ended successfully." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
