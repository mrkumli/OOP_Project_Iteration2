#ifndef DOORS_H
#define DOORS_H

#include <SFML/Graphics.hpp>
#include "Character.h"

class Doors {
protected:
    sf::FloatRect m_rect;
    sf::Sprite m_doorSprite;
    sf::Sprite m_frameSprite;
    sf::Sprite m_backgroundSprite;
    sf::Texture m_doorTexture;
    sf::Texture m_frameTexture;
    sf::Texture m_backgroundTexture;

    bool m_isOpen;
    float m_heightRaised;
    bool m_playerAtDoor;
    sf::Vector2f m_doorLocation;

    static constexpr int CHUNK_SIZE = 16;
    static constexpr float DOOR_SPEED = 1.5f;

public:
    Doors(const sf::Vector2f& doorLocation);
    virtual ~Doors() = default;

    virtual void tryOpen(Character& player) = 0;
    void tryRaiseDoor();
    void draw(sf::RenderWindow& window);

    bool isOpen() const;
    sf::FloatRect getRect() const;

protected:
    void loadCommonImages();
};

class FireDoor : public Doors {
public:
    FireDoor(const sf::Vector2f& doorLocation);
    void tryOpen(Character& player) override;
};

class WaterDoor : public Doors {
public:
    WaterDoor(const sf::Vector2f& doorLocation);
    void tryOpen(Character& player) override;
};

#endif // DOORS_H
