#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <SFML/Graphics.hpp>
#include "Character.h"

class Controller {
public:
    virtual ~Controller() = default;
    virtual void controlPlayer(const sf::Event& event, Character& player) = 0;
};

class ArrowsController : public Controller {
public:
    void controlPlayer(const sf::Event& event, Character& player) override;
};

class WASDController : public Controller {
public:
    void controlPlayer(const sf::Event& event, Character& player) override;
};

#endif // CONTROLLER_H
