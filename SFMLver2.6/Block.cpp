#include "Block.h"

Block::Block(sf::Vector2f pos, BlockType blockType)
    : GameObject(pos, 100.0f), type(blockType) {

    shape.setSize(sf::Vector2f(40.0f, 40.0f));
    shape.setPosition(pos);
    shape.setOrigin(20.0f, 20.0f);

    switch (type) {
    case BlockType::DESTRUCTIBLE:
        originalColor = sf::Color(139, 69, 19);
        health = Constants::DESTRUCTIBLE_HEALTH;
        if (loadTreeTexture("assets/tree.png")) {
            treeSprite.setTexture(treeTexture);
            treeSprite.setPosition(pos);
            treeSprite.setOrigin(20.0f, 20.0f);
            treeSprite.setScale(40.0f / treeTexture.getSize().x, 40.0f / treeTexture.getSize().y);
        }
        break;
    case BlockType::WATER:
        originalColor = sf::Color(0, 100, 200);
        health = Constants::WALL_HEALTH;
        if (loadWaterTexture("assets/oil.png")) {
            waterSprite.setTexture(waterTexture);
            waterSprite.setPosition(pos);
            waterSprite.setOrigin(20.0f, 20.0f);
            waterSprite.setScale(40.0f / waterTexture.getSize().x, 40.0f / waterTexture.getSize().y);
        }
        break;
    case BlockType::WALL_INDESTRUCTIBLE:
        originalColor = sf::Color(100, 100, 100);
        health = 999999.0f; // Very high health to make it indestructible
        if (loadWallTexture("assets/wall.png")) {
            wallSprite.setTexture(wallTexture);
            wallSprite.setPosition(pos);
            wallSprite.setOrigin(20.0f, 20.0f);
            wallSprite.setScale(40.0f / wallTexture.getSize().x, 40.0f / wallTexture.getSize().y);
        }
        break;
    }
    shape.setFillColor(originalColor);
}

void Block::update(float deltaTime) {
    if (type == BlockType::DESTRUCTIBLE && health > 0) {
        float healthRatio = health / maxHealth;
        sf::Color damagedColor = sf::Color(
            static_cast<sf::Uint8>(originalColor.r * healthRatio),
            static_cast<sf::Uint8>(originalColor.g * healthRatio),
            static_cast<sf::Uint8>(originalColor.b * healthRatio)
        );
        shape.setFillColor(damagedColor);
    }


}

void Block::draw(sf::RenderWindow& window) {
    if (!destroyed) {
        switch (type) {
        case BlockType::DESTRUCTIBLE:
            if (treeTexture.getSize().x > 0) {
                window.draw(treeSprite);
            }
            else {
                window.draw(shape);
            }
            break;
        case BlockType::WATER:
            if (waterTexture.getSize().x > 0) {
                window.draw(waterSprite);
            }
            else {
                window.draw(shape);
            }
            break;
        case BlockType::WALL_INDESTRUCTIBLE:
            if (wallTexture.getSize().x > 0) {
                window.draw(wallSprite);
            }
            else {
                window.draw(shape);
            }
            break;
        default:
            window.draw(shape);
            break;
        }
    }
}

sf::FloatRect Block::getBounds() const {
    return shape.getGlobalBounds();
}

void Block::takeDamage(float damage) {
    if (type == BlockType::DESTRUCTIBLE) {
        GameObject::takeDamage(damage);
    }
}

bool Block::isBlocking() const {
    return !destroyed && (type == BlockType::DESTRUCTIBLE || type == BlockType::WALL_INDESTRUCTIBLE);
}

BlockType Block::getType() const {
    return type;
}

bool Block::loadTreeTexture(const std::string& texturePath) {
    return treeTexture.loadFromFile(texturePath);
}

bool Block::loadWaterTexture(const std::string& texturePath) {
    return waterTexture.loadFromFile(texturePath);
}

bool Block::loadWallTexture(const std::string& texturePath) {
    return wallTexture.loadFromFile(texturePath);
}
