#include "Bullet.h"

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f direction, float radius,
    sf::Color color, float spd, float dmg, bool playerOwned)
    : GameObject(pos, 1.0f), speed(spd), damage(dmg), isPlayerBullet(playerOwned), useTexture(false) {

    float len = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (len != 0) {
        velocity = sf::Vector2f(direction.x / len * speed, direction.y / len * speed);
    }

    shape.setRadius(radius);
    shape.setFillColor(color);
    shape.setOrigin(radius, radius);
    shape.setPosition(pos);
    position = pos;
}

Bullet::Bullet(sf::Vector2f pos, sf::Vector2f direction, float radius,
    const std::string& texturePath, float spd, float dmg, bool playerOwned)
    : GameObject(pos, 1.0f), speed(spd), damage(dmg), isPlayerBullet(playerOwned), useTexture(false) {

    float len = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (len != 0) {
        velocity = sf::Vector2f(direction.x / len * speed, direction.y / len * speed);
    }

    shape.setRadius(radius);
    shape.setFillColor(sf::Color::White); // Default color for texture
    shape.setOrigin(radius, radius);
    shape.setPosition(pos);
    position = pos;

    // Try to load texture
    if (loadTexture(texturePath)) {
        useTexture = true;
        bulletSprite.setOrigin(radius, radius);
        bulletSprite.setPosition(pos);
        bulletSprite.setScale(radius * 2.0f / bulletTexture.getSize().x,
            radius * 2.0f / bulletTexture.getSize().y);
    }
}

void Bullet::update(float deltaTime) {
    if (!destroyed) {
        position += velocity * deltaTime;
        shape.setPosition(position);
        if (useTexture) {
            bulletSprite.setPosition(position);
        }

        if (position.x < 0 || position.x > Constants::WINDOW_WIDTH ||
            position.y < 0 || position.y > Constants::WINDOW_HEIGHT) {
            destroyed = true;
        }
    }
}

void Bullet::draw(sf::RenderWindow& window) {
    if (!destroyed) {
        if (useTexture) {
            window.draw(bulletSprite);
        }
        else {
            window.draw(shape);
        }
    }
}

sf::FloatRect Bullet::getBounds() const {
    if (useTexture) {
        return bulletSprite.getGlobalBounds();
    }
    return shape.getGlobalBounds();
}

void Bullet::move(float dx, float dy) {
    position.x += dx;
    position.y += dy;
    shape.setPosition(position);
    if (useTexture) {
        bulletSprite.setPosition(position);
    }
}

void Bullet::setPosition(sf::Vector2f pos) {
    position = pos;
    shape.setPosition(pos);
    if (useTexture) {
        bulletSprite.setPosition(pos);
    }
}

sf::Vector2f Bullet::getVelocity() const {
    return velocity;
}

float Bullet::getDamage() const {
    return damage;
}

bool Bullet::isFromPlayer() const {
    return isPlayerBullet;
}

bool Bullet::loadTexture(const std::string& texturePath) {
    return bulletTexture.loadFromFile(texturePath);
}

void Bullet::setUseTexture(bool use) {
    useTexture = use;
}