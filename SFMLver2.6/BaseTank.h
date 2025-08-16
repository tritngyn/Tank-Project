#pragma once
#include "GameObject.h"
#include "IMoveable.h"
#include "ICombat.h"
#include "Bullet.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <memory>

class BaseTank : public GameObject, public IMoveable, public ICombat {
protected:
    sf::RectangleShape body;
    sf::Vector2f velocity;
    float speed;
    float rotation;
    sf::Clock attackCooldown;
    float maxAttackCooldown;
    std::vector<std::unique_ptr<Bullet>>* bulletContainer;

    virtual void createBullet(sf::Vector2f target) = 0;
    virtual bool canMoveTo(sf::Vector2f newPos, const std::vector<std::unique_ptr<class Block>>& blocks);

public:
    BaseTank(sf::Vector2f pos, float spd, sf::Color color,
        std::vector<std::unique_ptr<Bullet>>* bullets);
    virtual ~BaseTank() = default;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    void move(float dx, float dy) override;
    void setPosition(sf::Vector2f pos) override;
    sf::Vector2f getVelocity() const override;

    bool canAttack() const override;
    void attack(sf::Vector2f target) override;
    void takeDamage(float damage) override;
    float getHealth() const override;
    float getMaxHealth() const override;

    virtual void drawHealthBar(sf::RenderWindow& window);
    virtual void setRotation(float angle);
    float getRotation() const;
    void updateMovement(float deltaTime, const std::vector<std::unique_ptr<class Block>>& blocks);

    void setSpeed(float s) { speed = s; }
    void setAttackCooldown(float cooldown) { maxAttackCooldown = cooldown; }
};