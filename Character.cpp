#include "include/Character.h"
#include "include/Board.h"
#include <iostream>

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

    calcMovement();
    handleCollisions(board);
}

void Character::calcMovement() {
    const float LATERAL_SPEED = 3.0f;
    const float JUMP_SPEED = -5.0f;
    const float GRAVITY = 0.2f;
    const float TERMINAL_VELOCITY = 3.0f;

    if (m_movingRight) {
        m_rect.position.x += LATERAL_SPEED;
    }
    if (m_movingLeft) {
        m_rect.position.x -= LATERAL_SPEED;
    }

    if (m_isJumping) {
        m_yVelocity = JUMP_SPEED;
        m_isJumping = false;
    }

    m_rect.position.y += m_yVelocity;
    m_yVelocity += GRAVITY;

    if (m_yVelocity > TERMINAL_VELOCITY) {
        m_yVelocity = TERMINAL_VELOCITY;
    }
}

void Character::handleCollisions(Board& board) {
    const auto& solidBlocks = board.getSolidBlocks();

    bool collisionBottom = false;
    bool collisionTop = false;

    for (const auto& block : solidBlocks) {
        auto intersection = m_rect.findIntersection(block);
        if (intersection) {
            sf::FloatRect overlap = *intersection;

            // Determine if collision is more vertical or horizontal
            if (overlap.size.x > overlap.size.y) {
                // Vertical collision (top or bottom)

                // Character is ABOVE the block (standing on it)
                if (m_rect.position.y < block.position.y) {
                    // Keep character on TOP of the block
                    m_rect.position.y = block.position.y - m_rect.size.y;
                    m_yVelocity = 0.0f;
                    collisionBottom = true;
                }
                // Character is BELOW the block (hitting ceiling)
                else {
                    // Push character DOWN below the block
                    m_rect.position.y = block.position.y + block.size.y;
                    m_yVelocity = 0.0f;
                    collisionTop = true;
                }
            } else {
                // Horizontal collision (left or right)

                // Character is to the LEFT of the block
                if (m_rect.position.x < block.position.x) {
                    m_rect.position.x = block.position.x - m_rect.size.x;
                }
                // Character is to the RIGHT of the block
                else {
                    m_rect.position.x = block.position.x + block.size.x;
                }
            }
        }
    }

    if (collisionBottom) {
        m_airTimer = 0;
    } else {
        m_airTimer++;
    }
}

void Character::draw(sf::RenderWindow& window) {
    if (!m_isAlive || !m_sprite) return;

    m_sprite->setPosition(m_rect.position);
    window.draw(*m_sprite);
}

void Character::kill() {
    m_isAlive = false;
    std::cout << m_type << " player died!" << std::endl;
}

bool Character::isDead() const {
    return !m_isAlive;
}

sf::FloatRect Character::getRect() const {
    return m_rect;
}

std::string Character::getType() const {
    return m_type;
}

void Character::setMovingRight(bool moving) {
    m_movingRight = moving;
}

void Character::setMovingLeft(bool moving) {
    m_movingLeft = moving;
}

void Character::setJumping(bool jumping) {
    if (m_airTimer < 6) {
        m_isJumping = jumping;
    }
}

Hot::Hot(const sf::Vector2f& pos) : Character(pos) {
    m_type = "hot";

    if (!m_texture.loadFromFile("data/player_images/magmaboy.png")) {
        std::cerr << "Failed to load Hot player texture, using fallback" << std::endl;
        sf::Image fallback(sf::Vector2u(16, 32), sf::Color::Red);
        if (!m_texture.loadFromImage(fallback)) {
            std::cerr << "Failed to create fallback texture" << std::endl;
        }
    }

    m_sprite.emplace(m_texture);
    m_rect.size = sf::Vector2f(static_cast<float>(m_texture.getSize().x),
                               static_cast<float>(m_texture.getSize().y));
}

void Hot::update(Board& board) {
    Character::update(board);
}

Cold::Cold(const sf::Vector2f& pos) : Character(pos) {
    m_type = "cold";

    if (!m_texture.loadFromFile("data/player_images/hydrogirl.png")) {
        std::cerr << "Failed to load Cold player texture, using fallback" << std::endl;
        sf::Image fallback(sf::Vector2u(16, 32), sf::Color::Blue);
        if (!m_texture.loadFromImage(fallback)) {
            std::cerr << "Failed to create fallback texture" << std::endl;
        }
    }

    m_sprite.emplace(m_texture);
    m_rect.size = sf::Vector2f(static_cast<float>(m_texture.getSize().x),
                               static_cast<float>(m_texture.getSize().y));
}

void Cold::update(Board& board) {
    Character::update(board);
}
