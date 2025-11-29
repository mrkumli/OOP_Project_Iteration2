#ifndef CHARACTER_H
#define CHARACTER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

class Board;

class Character {
protected:
    sf::FloatRect m_rect;
    std::optional<sf::Sprite> m_sprite;
    sf::Texture m_texture;
    bool m_isAlive;
    float m_yVelocity;
    bool m_isJumping;
    bool m_movingRight;
    bool m_movingLeft;
    int m_airTimer;

    std::string m_type;

public:
    Character(const sf::Vector2f& pos);
    virtual ~Character() = default;

    virtual void update(Board& board);
    virtual void draw(sf::RenderWindow& window);
    virtual void kill();

    bool isDead() const;
    sf::FloatRect getRect() const;
    std::string getType() const;

    // NEW: Add setPosition for gate collision
    void setPosition(const sf::Vector2f& pos);
    void setRect(const sf::FloatRect& rect);

    void setMovingRight(bool moving);
    void setMovingLeft(bool moving);
    void setJumping(bool jumping);

protected:
    void calcMovement();
    void handleCollisions(Board& board);
};

class Hot : public Character {
public:
    Hot(const sf::Vector2f& pos);
    void update(Board& board) override;
};

class Cold : public Character {
public:
    Cold(const sf::Vector2f& pos);
    void update(Board& board) override;
};

#endif // CHARACTER_H
