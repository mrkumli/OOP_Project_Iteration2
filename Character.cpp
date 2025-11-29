#include "include/Character.h"
#include "include/Board.h"
#include <iostream>
#include <cmath>

Character::Character(const sf::Vector2f& pos)
    : m_isAlive(true),
      m_yVelocity(0.0f),
      m_isJumping(false),
      m_movingRight(false),
      m_movingLeft(false),
      m_airTimer(0)
{
    m_rect = sf::FloatRect(pos, sf::Vector2f(16.0f, 32.0f));
}

void Character::update(Board& board) {
    if (!m_isAlive) return;

    const float SPEED = 3.0f;
    const float GRAVITY = 0.3f;
    const float JUMP_FORCE = -7.5f;
    const float MAX_FALL_SPEED = 8.0f;

    sf::Vector2f velocity(0.0f, m_yVelocity);

    if (m_movingRight) velocity.x += SPEED;
    if (m_movingLeft)  velocity.x -= SPEED;

    if (m_isJumping && m_airTimer < 5) {
        m_yVelocity = JUMP_FORCE;
        m_isJumping = false;
    }

    m_yVelocity += GRAVITY;
    if (m_yVelocity > MAX_FALL_SPEED) m_yVelocity = MAX_FALL_SPEED;
    velocity.y = m_yVelocity;

    // X AXIS MOVEMENT
    m_rect.position.x += velocity.x;

    const auto& solidBlocks = board.getSolidBlocks();
    for (const auto& block : solidBlocks) {
        if (auto intersection = m_rect.findIntersection(block)) {
            sf::FloatRect overlap = *intersection;

            if (overlap.size.x < overlap.size.y) {
                if (m_rect.position.x < block.position.x) {
                    m_rect.position.x -= overlap.size.x;
                } else {
                    m_rect.position.x += overlap.size.x;
                }
            }
        }
    }

    // Y AXIS MOVEMENT
    m_rect.position.y += velocity.y;

    bool onGround = false;
    for (const auto& block : solidBlocks) {
        if (auto intersection = m_rect.findIntersection(block)) {
            sf::FloatRect overlap = *intersection;

            if (overlap.size.x > overlap.size.y) {
                if (m_rect.position.y < block.position.y) {
                    m_rect.position.y -= overlap.size.y;
                    m_yVelocity = 0.0f;
                    onGround = true;
                } else {
                    m_rect.position.y += overlap.size.y;
                    m_yVelocity = 0.0f;
                }
            }
        }
    }

    if (onGround) {
        m_airTimer = 0;
    } else {
        m_airTimer++;
    }
}

void Character::calcMovement() {}
void Character::handleCollisions(Board& board) {}

void Character::draw(sf::RenderWindow& window) {
    if (!m_isAlive || !m_sprite) return;

    m_sprite->setPosition(m_rect.position);
    window.draw(*m_sprite);
}

void Character::kill() {
    m_isAlive = false;
    std::cout << m_type << " player died!" << std::endl;
}

bool Character::isDead() const { return !m_isAlive; }
sf::FloatRect Character::getRect() const { return m_rect; }
std::string Character::getType() const { return m_type; }

// NEW METHODS FOR GATE COLLISION
void Character::setPosition(const sf::Vector2f& pos) {
    m_rect.position = pos;
}

void Character::setRect(const sf::FloatRect& rect) {
    m_rect = rect;
}

void Character::setMovingRight(bool moving) { m_movingRight = moving; }
void Character::setMovingLeft(bool moving) { m_movingLeft = moving; }
void Character::setJumping(bool jumping) { m_isJumping = jumping; }

Hot::Hot(const sf::Vector2f& pos) : Character(pos) {
    m_type = "hot";
    if (m_texture.loadFromFile("data/player_images/magmaboy.png")) {
        // loaded
    }
    m_sprite.emplace(m_texture);

    if (m_texture.getSize().x > 0) {
        m_rect.size = sf::Vector2f(static_cast<float>(m_texture.getSize().x),
                                   static_cast<float>(m_texture.getSize().y));
    }
}
void Hot::update(Board& board) { Character::update(board); }

Cold::Cold(const sf::Vector2f& pos) : Character(pos) {
    m_type = "cold";
    if (m_texture.loadFromFile("data/player_images/hydrogirl.png")) {
        // loaded
    }
    m_sprite.emplace(m_texture);

    if (m_texture.getSize().x > 0) {
        m_rect.size = sf::Vector2f(static_cast<float>(m_texture.getSize().x),
                                   static_cast<float>(m_texture.getSize().y));
    }
}
void Cold::update(Board& board) { Character::update(board); }
