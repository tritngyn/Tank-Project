#include "Background.h"
#include <iostream>
Background::Background() : parallaxFactor(0.1f) {
    tileSize = sf::Vector2u(100, 100); // Default tile size
    windowSize = sf::Vector2u(800, 600); // Default window size
}

bool Background::loadBackground(const std::string& texturePath) {
    if (!backgroundTexture.loadFromFile(texturePath)) {
        std::cout << "Failed to load background texture: " << texturePath << std::endl;
        return false;
    }

    tileSize = backgroundTexture.getSize();
    std::cout << "Background texture loaded: " << tileSize.x << "x" << tileSize.y << std::endl;

    // Create tiled background
    tiledSprites.clear();
    unsigned int tilesX = (windowSize.x / tileSize.x) + 2;
    unsigned int tilesY = (windowSize.y / tileSize.y) + 2;

    for (unsigned int y = 0; y < tilesY; ++y) {
        for (unsigned int x = 0; x < tilesX; ++x) {
            sf::Sprite tile;
            tile.setTexture(backgroundTexture);
            tile.setPosition(
                static_cast<float>(x * tileSize.x),
                static_cast<float>(y * tileSize.y)
            );
            tiledSprites.push_back(tile);
        }
    }

    return true;
}

void Background::setParallaxFactor(float factor) {
    parallaxFactor = factor;
}

void Background::update(float deltaTime) {
    // Update parallax effect if needed
    // This can be enhanced with camera movement
}

void Background::draw(sf::RenderWindow& window) {
    // Draw all tiled 
    for (const auto& sprite : tiledSprites) {
        window.draw(sprite);
    }
}

void Background::resize(sf::Vector2u newSize) {
    windowSize = newSize;

    // Recalculate tiles needed
    unsigned int tilesX = (windowSize.x / tileSize.x) + 2;
    unsigned int tilesY = (windowSize.y / tileSize.y) + 2;

    tiledSprites.clear();
    for (unsigned int y = 0; y < tilesY; ++y) {
        for (unsigned int x = 0; x < tilesX; ++x) {
            sf::Sprite tile;
            tile.setTexture(backgroundTexture);
            tile.setPosition(
                static_cast<float>(x * tileSize.x),
                static_cast<float>(y * tileSize.y)
            );
            tiledSprites.push_back(tile);
        }
    }
}
