#pragma once
#include <SFML/Graphics.hpp>

class IMoveable {
public:
    virtual ~IMoveable() = default;
    virtual void move(float dx, float dy) = 0;
    virtual void setPosition(sf::Vector2f pos) = 0;
    virtual sf::Vector2f getVelocity() const = 0;
};