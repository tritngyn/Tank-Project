#pragma once
#include "GameObject.h"
#include "Constants.h"
#include <SFML/Graphics.hpp>

enum class BlockType {
    DESTRUCTIBLE,  // Tree blocks
    WATER,
    WALL_INDESTRUCTIBLE  // Wall blocks that cannot be destroyed
};

class Block : public GameObject {
private:
    sf::RectangleShape shape;
    BlockType type;
    sf::Color originalColor;

    // Texture support for different block types
    sf::Texture treeTexture;
    sf::Sprite treeSprite;
    sf::Texture waterTexture;
    sf::Sprite waterSprite;
    sf::Texture wallTexture;
    sf::Sprite wallSprite;

    // Water texture support (no animation)

public:
    Block(sf::Vector2f pos, BlockType blockType);

    void update(float deltaTime) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;

    bool checkGrass(float x, float y);
    void takeDamage(float damage) override;
    bool isBlocking() const;
    BlockType getType() const;

    // Texture loading methods
    bool loadTreeTexture(const std::string& texturePath);
    bool loadWaterTexture(const std::string& texturePath);
    bool loadWallTexture(const std::string& texturePath);


};