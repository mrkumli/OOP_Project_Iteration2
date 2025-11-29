#ifndef GATES_H
#define GATES_H

#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <optional>
#include "Character.h"

class Gates {
private:
    // Member variables in order of initialization
    sf::Vector2f m_gateLocation;
    std::vector<sf::Vector2f> m_plateLocations;
    bool m_isPressed;
    bool m_isOpen;

    sf::FloatRect m_gateRect;
    std::vector<sf::FloatRect> m_plateRects;
    std::optional<sf::Sprite> m_gateSprite;
    std::vector<sf::Sprite> m_plateSprites;
    sf::Texture m_gateTexture;
    sf::Texture m_plateTexture;

    static constexpr int CHUNK_SIZE = 16;

public:
    Gates(const sf::Vector2f& gateLocation, const std::vector<sf::Vector2f>& plateLocations);

    void tryOpen(const std::list<Character*>& players);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getGateRect() const;
    const std::vector<sf::FloatRect>& getPlateRects() const;
    bool isOpen() const;

private:
    void loadImages();
    bool checkCollision(const sf::FloatRect& rect1, const sf::FloatRect& rect2) const;
};

#endif // GATES_H
