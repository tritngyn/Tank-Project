#pragma once
#include "Constants.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Bullet.h"
#include "Block.h"
#include "Background.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include <string>

enum class GameState { MENU, PLAYING, PAUSED, GAMEOVER, INSTRUCTIONS };

class Game {
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::SoundBuffer shootBuffer, hitBuffer;
    sf::Sound shootSound, hitSound;
    sf::Music bgMusic;
    sf::Text pauseResumeText;
    sf::Text pauseMenuText;

    GameState currentState;

    std::unique_ptr<PlayerTank> player;
    std::vector<std::unique_ptr<EnemyTank>> enemies;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::vector<std::unique_ptr<Block>> levelBlocks;
    std::unique_ptr<Background> background;

    sf::Text titleText, scoreText, gameOverText;
    std::vector<sf::Text> menuOptions;
    std::vector<sf::Text> instructionTexts;

    int score;
    int level;
    std::vector<int> scoreHistory;
    sf::Clock gameTimer, enemySpawnTimer;
    float enemySpawnInterval;

    int selectedOption;

    int enemiesToDefeat;
    int enemiesDefeated;

    bool showLevelNotification = false;
    sf::Text levelNotificationText;
    sf::Clock levelNotificationTimer;
    sf::Text levelHintText;

    float levelNotificationDuration = 2.0f;
    bool mouseLeftPreviouslyPressed = false;
    std::vector<int> highScores;

public:
    Game();
    void run();

    static std::vector<int> loadHighScores();
    static void saveHighScores(const std::vector<int>& scores);
private:
    void initializeFont();
    void setupUI();
    void setupLevel();
    void createLevelBlocks();
    void setupBackground();

    void handleEvents();
    void handleStateSpecificEvents(const sf::Event& event);
    void handleMenuEvents(const sf::Event& event);
    void executeMenuOption();
    void handleGameplayInput();

    void startGame();
    void restartGame();
    void update(float deltaTime);
    void spawnEnemies();
    sf::Vector2f getRandomSpawnPosition();
    void checkCollisions();
    void cleanupObjects();
    void nextLevel();

    void render();
    void renderMenu();
    void renderBackground();
    void renderGame();
    void renderGameUI();
    void renderPause();
    void renderGameOver();
    void renderInstructions();
    void centerText(sf::Text& text, float x, float y);
};
