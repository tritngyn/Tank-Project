#pragma once
#include "GameObject.h"
#include "IMoveable.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <cmath>

class Bullet : public GameObject, public IMoveable {
protected:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speed;
    float damage;
    bool isPlayerBullet;

    // Texture support
    sf::Texture bulletTexture;
    sf::Sprite bulletSprite;
    bool useTexture;

public:
    Bullet(sf::Vector2f pos, sf::Vector2f direction, float radius,
        sf::Color color, float spd, float dmg, bool playerOwned);

    // Constructor with texture
    Bullet(sf::Vector2f pos, sf::Vector2f direction, float radius,
        const std::string& texturePath, float spd, float dmg, bool playerOwned);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    void move(float dx, float dy) override;
    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getVelocity() const override;

    float getDamage() const;
    bool isFromPlayer() const;

    // Texture methods
    bool loadTexture(const std::string& texturePath);
    void setUseTexture(bool use);
};