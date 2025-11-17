#include "Controller.h"

// ArrowsController - Controls Hot Player
void ArrowsController::controlPlayer(const sf::Event& event, Character& player) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::Right:
                player.setMovingRight(true);
                break;
            case sf::Keyboard::Key::Left:
                player.setMovingLeft(true);
                break;
            case sf::Keyboard::Key::Up:
                player.setJumping(true);
                break;
            default:
                break;
        }
    }
    else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        switch (keyReleased->code) {
            case sf::Keyboard::Key::Right:
                player.setMovingRight(false);
                break;
            case sf::Keyboard::Key::Left:
                player.setMovingLeft(false);
                break;
            case sf::Keyboard::Key::Up:
                player.setJumping(false);
                break;
            default:
                break;
        }
    }
}

// WASDController - Controls Cold Player
void WASDController::controlPlayer(const sf::Event& event, Character& player) {
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->code) {
            case sf::Keyboard::Key::D:
                player.setMovingRight(true);
                break;
            case sf::Keyboard::Key::A:
                player.setMovingLeft(true);
                break;
            case sf::Keyboard::Key::W:
                player.setJumping(true);
                break;
            default:
                break;
        }
    }
    else if (const auto* keyReleased = event.getIf<sf::Event::KeyReleased>()) {
        switch (keyReleased->code) {
            case sf::Keyboard::Key::D:
                player.setMovingRight(false);
                break;
            case sf::Keyboard::Key::A:
                player.setMovingLeft(false);
                break;
            case sf::Keyboard::Key::W:
                player.setJumping(false);
                break;
            default:
                break;
        }
    }
}
