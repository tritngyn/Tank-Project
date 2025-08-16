#pragma once
#include <SFML/Graphics.hpp>

class ICombat {
public:
    virtual ~ICombat() = default;
    virtual void takeDamage(float damage) = 0;
    virtual bool canAttack() const = 0;
    virtual void attack(sf::Vector2f target) = 0;
    virtual float getHealth() const = 0;
    virtual float getMaxHealth() const = 0;
};