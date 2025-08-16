#include "BaseTank.h"
#include "Block.h"
#include <algorithm>

BaseTank::BaseTank(sf::Vector2f pos, float spd, sf::Color color,
    std::vector<std::unique_ptr<Bullet>>* bullets)
    : GameObject(pos, Constants::PLAYER_HEALTH), speed(spd), rotation(0.0f),
    maxAttackCooldown(Constants::PLAYER_ATTACK_COOLDOWN), bulletContainer(bullets) {

    body.setSize(sf::Vector2f(35.0f, 35.0f));
    body.setFillColor(color);
    body.setOrigin(17.5f, 17.5f);
    body.setPosition(pos);
    velocity = sf::Vector2f(0, 0);
}

void BaseTank::update(float deltaTime) {
    if (!destroyed) {
        position = body.getPosition();
    }
}

void BaseTank::draw(sf::RenderWindow& window) {
    if (!destroyed) {
        window.draw(body);
        drawHealthBar(window);
    }
}

void BaseTank::drawHealthBar(sf::RenderWindow& window) {
    if (destroyed) return;

    sf::RectangleShape healthBg;
    healthBg.setSize(sf::Vector2f(40.0f, 6.0f));
    healthBg.setPosition(position.x - 20, position.y - 30);
    healthBg.setFillColor(sf::Color::Red);
    window.draw(healthBg);

    sf::RectangleShape healthBar;
    float healthRatio = health / maxHealth;
    healthBar.setSize(sf::Vector2f(40.0f * healthRatio, 6.0f));
    healthBar.setPosition(position.x - 20, position.y - 30);
    healthBar.setFillColor(sf::Color::Green);
    window.draw(healthBar);
}

sf::FloatRect BaseTank::getBounds() const {
    return body.getGlobalBounds();
}

void BaseTank::move(float dx, float dy) {
    velocity = sf::Vector2f(dx * speed, dy * speed);
}

void BaseTank::setPosition(sf::Vector2f pos) {
    position = pos;
    body.setPosition(pos);
}

sf::Vector2f BaseTank::getVelocity() const {
    return velocity;
}

bool BaseTank::canAttack() const {
    return attackCooldown.getElapsedTime().asSeconds() >= maxAttackCooldown;
}

void BaseTank::attack(sf::Vector2f target) {
    if (canAttack() && !destroyed) {
        createBullet(target);
        attackCooldown.restart();
    }
}

void BaseTank::takeDamage(float damage) {
    GameObject::takeDamage(damage);
}

float BaseTank::getHealth() const {
    return health;
}

float BaseTank::getMaxHealth() const {
    return maxHealth;
}

void BaseTank::setRotation(float angle) {
    rotation = angle;
    body.setRotation(angle);
}

float BaseTank::getRotation() const {
    return rotation;
}

bool BaseTank::canMoveTo(sf::Vector2f newPos, const std::vector<std::unique_ptr<Block>>& blocks) {
    sf::FloatRect futurePos;
    futurePos.left = newPos.x - 17.5f;
    futurePos.top = newPos.y - 17.5f;
    futurePos.width = 35.0f;
    futurePos.height = 35.0f;

    for (const auto& block : blocks) {
        if (block && !block->isDestroyed() && block->isBlocking()) {
            if (futurePos.intersects(block->getBounds())) {
                return false;
            }
        }
    }
    return true;
}

void BaseTank::updateMovement(float deltaTime, const std::vector<std::unique_ptr<Block>>& blocks) {
    if (destroyed) return;

    sf::Vector2f newPos = position + velocity * deltaTime;

    newPos.x = std::max(20.0f, std::min(780.0f, newPos.x));
    newPos.y = std::max(20.0f, std::min(580.0f, newPos.y));

    if (canMoveTo(newPos, blocks)) {
        setPosition(newPos);
    }
}