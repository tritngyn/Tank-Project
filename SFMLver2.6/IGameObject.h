#pragma once
#include <SFML/Graphics.hpp>

class IGameObject {
public:
    virtual ~IGameObject() = default;
    virtual void update(float deltaTime) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual sf::Vector2f getCenter() const = 0;
    virtual bool isDestroyed() const = 0;
};