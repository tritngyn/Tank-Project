#pragma once
#include "BaseTank.h"
#include <SFML/Graphics.hpp>

class PlayerTank;

class EnemyTank : public BaseTank {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    bool useTexture;

    PlayerTank* player;
    float aiTimer;
    sf::Vector2f targetDirection;

public:
    EnemyTank(sf::Vector2f pos, PlayerTank* playerRef, std::vector<std::unique_ptr<Bullet>>* bullets);

    void update(float deltaTime) override;

    void updateMovement(float deltaTime, const std::vector<std::unique_ptr<class Block>>& blocks);

    void draw(sf::RenderWindow& window) override;

protected:
    void createBullet(sf::Vector2f target) override;
};