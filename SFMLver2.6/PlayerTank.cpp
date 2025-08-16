#include "PlayerTank.h"
#include <iostream>

PlayerTank::PlayerTank(sf::Vector2f pos, std::vector<std::unique_ptr<Bullet>>* bullets)
    : BaseTank(pos, Constants::PLAYER_SPEED, sf::Color::Green, bullets), useTexture(false) {

    maxAttackCooldown = Constants::PLAYER_ATTACK_COOLDOWN;

    if (texture.loadFromFile("assets/Tank.png")) {
        useTexture = true;
        sprite.setTexture(texture);
        sprite.setScale(1.f, 1.f);
        sprite.setOrigin(static_cast<float>(texture.getSize().x) / 2.0f,
            static_cast<float>(texture.getSize().y) / 2.0f);
        sprite.setPosition(pos);
    }
}

void PlayerTank::update(float deltaTime) {
    if (!destroyed && useTexture) {
        sprite.setPosition(position);
        sprite.setRotation(rotation + 90); 
    }
    BaseTank::update(deltaTime);
}

void PlayerTank::draw(sf::RenderWindow& window) {
    if (!destroyed) {
        if (useTexture) {
            window.draw(sprite);
        }
        else {
            window.draw(body);
        }
        drawHealthBar(window);
    }
}

void PlayerTank::setRotation(float angle) {
    BaseTank::setRotation(angle);
    if (useTexture) {
        sprite.setRotation(angle + 90.0f);
    }
}

void PlayerTank::createBullet(sf::Vector2f target) {
    sf::Vector2f direction = target - position;
    auto bullet = std::make_unique<Bullet>(position, direction, 6.0f,
        sf::Color::Yellow, Constants::BULLET_SPEED,
        Constants::PLAYER_BULLET_DAMAGE, true);
    bulletContainer->push_back(std::move(bullet));
}

void PlayerTank::setScale(float x, float y) {
        body.setScale(0.1f * x, 0.1f * y);
}

sf::FloatRect PlayerTank::getBounds() const {
    if (useTexture) {
        return sprite.getGlobalBounds();
    } else {
        return body.getGlobalBounds();
    }
}