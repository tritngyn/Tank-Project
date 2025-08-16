#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Background {
private:
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;

    // Tiled background
    std::vector<sf::Sprite> tiledSprites;
    sf::Vector2u tileSize;
    sf::Vector2u windowSize;

    // Parallax effect
    float parallaxFactor;

public:
    Background();
    bool loadBackground(const std::string& texturePath);
    void setParallaxFactor(float factor);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);
    void resize(sf::Vector2u newSize);
};
