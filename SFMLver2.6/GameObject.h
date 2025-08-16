#pragma once
#include "IGameObject.h"
#include <SFML/Graphics.hpp>

class GameObject : public IGameObject {
protected:
    sf::Vector2f position;
    bool destroyed;
    float health;
    float maxHealth;

public:
    GameObject(sf::Vector2f pos, float hp = 100.0f);
    virtual ~GameObject() = default;

    sf::Vector2f getCenter() const override;
    bool isDestroyed() const override;

    float getHealth() const;
    float getMaxHealth() const;
    virtual void destroy();
    virtual void takeDamage(float damage);
};