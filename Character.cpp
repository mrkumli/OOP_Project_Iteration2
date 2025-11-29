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
    // SFML 3: FloatRect takes (position, size)
    m_rect = sf::FloatRect(pos, sf::Vector2f(16.0f, 32.0f));
}

void Character::update(Board& board) {
    if (!m_isAlive) return;

    // --- PHYSICS CONSTANTS ---
    const float SPEED = 3.0f;
    const float GRAVITY = 0.3f;
    const float JUMP_FORCE = -7.5f;
    const float MAX_FALL_SPEED = 8.0f;

    // 1. CALCULATE VELOCITY
    sf::Vector2f velocity(0.0f, m_yVelocity);

    if (m_movingRight) velocity.x += SPEED;
    if (m_movingLeft)  velocity.x -= SPEED;

    // Jump Logic
    if (m_isJumping && m_airTimer < 5) {
        m_yVelocity = JUMP_FORCE;
        m_isJumping = false;
    }

    // Gravity
    m_yVelocity += GRAVITY;
    if (m_yVelocity > MAX_FALL_SPEED) m_yVelocity = MAX_FALL_SPEED;
    velocity.y = m_yVelocity;

    // --- PHASE 1: X AXIS MOVEMENT ---
    m_rect.position.x += velocity.x;

    // Check X Collisions
    const auto& solidBlocks = board.getSolidBlocks();
    for (const auto& block : solidBlocks) {
        // SFML 3: findIntersection returns std::optional
        if (auto intersection = m_rect.findIntersection(block)) {
            sf::FloatRect overlap = *intersection;

            // Only resolve if the overlap is horizontal (shallowest penetration)
            if (overlap.size.x < overlap.size.y) {
                if (m_rect.position.x < block.position.x) {
                    // Hit left side
                    m_rect.position.x -= overlap.size.x;
                } else {
                    // Hit right side
                    m_rect.position.x += overlap.size.x;
                }
            }
        }
    }

    // --- PHASE 2: Y AXIS MOVEMENT ---
    m_rect.position.y += velocity.y;

    // Check Y Collisions
    bool onGround = false;
    for (const auto& block : solidBlocks) {
        if (auto intersection = m_rect.findIntersection(block)) {
            sf::FloatRect overlap = *intersection;

            // Only resolve if the overlap is vertical
            if (overlap.size.x > overlap.size.y) {
                if (m_rect.position.y < block.position.y) {
                    // Landed on floor
                    m_rect.position.y -= overlap.size.y;
                    m_yVelocity = 0.0f;
                    onGround = true;
                } else {
                    // Hit ceiling
                    m_rect.position.y += overlap.size.y;
                    m_yVelocity = 0.0f;
                }
            }
        }
    }

    // Update air timer based on ground state
    if (onGround) {
        m_airTimer = 0;
    } else {
        m_airTimer++;
    }
}

// These methods are required by the header, but we moved their logic to update()
// We leave them empty to satisfy the compiler.
void Character::calcMovement() {}
void Character::handleCollisions(Board& board) {}

void Character::draw(sf::RenderWindow& window) {
    if (!m_isAlive || !m_sprite) return;

    // Sync sprite to physics rect
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

void Character::setMovingRight(bool moving) { m_movingRight = moving; }
void Character::setMovingLeft(bool moving) { m_movingLeft = moving; }
void Character::setJumping(bool jumping) { m_isJumping = jumping; }

Hot::Hot(const sf::Vector2f& pos) : Character(pos) {
    m_type = "hot";
    // Using simple fallback directly if file load fails
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
