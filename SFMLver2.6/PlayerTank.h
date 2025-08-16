#pragma once
#include "BaseTank.h"
#include <SFML/Graphics.hpp>

class PlayerTank : public BaseTank {
private:
    sf::Texture texture;
    sf::Sprite sprite;
    bool useTexture;

public:
    PlayerTank(sf::Vector2f pos, std::vector<std::unique_ptr<Bullet>>* bullets);

    sf::FloatRect getBounds() const override;

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;

    void setRotation(float angle) override;

    void setScale(float x, float y);

protected:
    void createBullet(sf::Vector2f target) override;
};