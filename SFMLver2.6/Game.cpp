#include "Game.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cstdio>
const std::string HIGH_SCORE_FILE = "highscores.txt";

Game::Game() : window(sf::VideoMode(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT), "Tank Battle - Modular Edition"),
currentState(GameState::MENU), score(0), level(1), selectedOption(0) {

    window.setFramerateLimit(60);
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    if (!shootBuffer.loadFromFile("audio/shoot.wav"))
        std::cerr << "Failed to load shoot.wav\n";
    if (!hitBuffer.loadFromFile("audio/hit.wav"))
        std::cerr << "Failed to load hit.wav\n";
    if (!bgMusic.openFromFile("audio/bg.wav"))
        std::cerr << "Failed to load bg.wav\n";

    shootSound.setBuffer(shootBuffer);
    hitSound.setBuffer(hitBuffer);
    bgMusic.setLoop(true);
    bgMusic.play();

    initializeFont();
    setupUI();
    setupBackground();
    setupLevel();

    enemySpawnInterval = Constants::ENEMY_SPAWN_INTERVAL;

    highScores = loadHighScores();
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = gameTimer.restart().asSeconds();

        handleEvents();

        if (currentState == GameState::PLAYING && !showLevelNotification) {
            update(deltaTime);
        }

        if (showLevelNotification && levelNotificationTimer.getElapsedTime().asSeconds() >= levelNotificationDuration) {
            showLevelNotification = false;
            if (level <= 5)
                setupLevel();
        }

        render();
    }
    std::remove(HIGH_SCORE_FILE.c_str());
}

void Game::initializeFont() {
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Warning: Could not load arial.ttf, using default font\n";
    }
}

void Game::setupUI() {
    titleText.setFont(font);
    titleText.setString("TANK BATTLE");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::White);
    centerText(titleText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, 100.0f);

    std::vector<std::string> options = { "PLAY", "INSTRUCTIONS", "QUIT" };
    menuOptions.clear();

    pauseResumeText.setFont(font);
    pauseResumeText.setString("Press ESC to resume");
    pauseResumeText.setCharacterSize(20);
    pauseResumeText.setFillColor(sf::Color::Yellow);
    centerText(pauseResumeText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f + 60);

    pauseMenuText.setFont(font);
    pauseMenuText.setString("Go back to menu");
    pauseMenuText.setCharacterSize(20);
    pauseMenuText.setFillColor(sf::Color::White);
    centerText(pauseMenuText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f + 100);

    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text optionText;
        optionText.setFont(font);
        optionText.setString(options[i]);
        optionText.setCharacterSize(32);
        optionText.setFillColor(sf::Color::White);
        centerText(optionText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f,
            250.0f + static_cast<float>(i) * 60.0f);
        menuOptions.push_back(optionText);
    }

    std::vector<std::string> instructions = {
        "CONTROLS:",
        "WASD - Move tank",
        "Mouse - Aim and shoot",
        "ESC - Pause game",
        "",
        "OBJECTIVE:",
        "Destroy enemy tanks",
        "Avoid enemy bullets",
        "Use blocks for cover",
        "",
        "Press ESC to return to menu"
    };

    instructionTexts.clear();
    for (size_t i = 0; i < instructions.size(); ++i) {
        sf::Text instrText;
        instrText.setFont(font);
        instrText.setString(instructions[i]);
        instrText.setCharacterSize(20);
        instrText.setFillColor(sf::Color::White);
        centerText(instrText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f,
            150.0f + static_cast<float>(i) * 30.0f);
        instructionTexts.push_back(instrText);
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);

    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    centerText(gameOverText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f,
        static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f);
}

void Game::centerText(sf::Text& text, float x, float y) {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    text.setPosition(x, y);
}

void Game::setupBackground() {
    background = std::make_unique<Background>();
    // Try to load sand background texture
    if (!background->loadBackground("assets/dirt.png")) {
        // Fallback to a simple colored background
        std::cout << "Warning: Could not load sand background texture\n";
    }
    else {
        // Resize background to match window size
        background->resize(sf::Vector2u(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT));
        std::cout << "Background setup complete" << std::endl;
    }
}

void Game::setupLevel() {
    levelBlocks.clear();
    createLevelBlocks();

    player = std::make_unique<PlayerTank>(sf::Vector2f(100, 100), &bullets);

    float scale = 1.0f + (level - 1) * 0.5f;
    player->setScale(scale, scale);

    float enemySpeed = Constants::ENEMY_SPEED;
    if (level >= 5) {
        enemySpeed += 40.0f;
    }
    else if (level >= 3) {
        enemySpeed += 20.0f;
    }
    for (auto& enemy : enemies) {
        enemy->setSpeed(enemySpeed);
        if (level >= 5) {
            enemySpeed += 20.0f;
            enemy->setAttackCooldown(Constants::ENEMY_ATTACK_COOLDOWN * 0.7f);
        }
    }

    enemies.clear();
    bullets.clear();

    enemiesToDefeat = 5 + (level - 1) * 2;
    enemiesDefeated = 0;

    enemySpawnTimer.restart();
}



void Game::createLevelBlocks() {
    // No wall boundaries - open world with sand background

    for (int i = 0; i < 10; ++i) {
        float x = 100 + static_cast<float>(rand() % (Constants::WINDOW_WIDTH - 200));
        float y = 100 + static_cast<float>(rand() % (Constants::WINDOW_HEIGHT - 200));
        levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(x, y), BlockType::DESTRUCTIBLE));
    }
    for (int i = 0; i < 10; ++i) {
        float x = 100 + static_cast<float>(rand() % (Constants::WINDOW_WIDTH - 200));
        float y = 100 + static_cast<float>(rand() % (Constants::WINDOW_HEIGHT - 200));
        levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(x, y), BlockType::DESTRUCTIBLE));
    }
    // Some strategic tree blocks for cover
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(280, 240), BlockType::DESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(320, 200), BlockType::DESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(440, 400), BlockType::DESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(400, 440), BlockType::DESTRUCTIBLE));

    // Add some indestructible wall blocks for strategic gameplay
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(200, 200), BlockType::WALL_INDESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(240, 200), BlockType::WALL_INDESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(600, 400), BlockType::WALL_INDESTRUCTIBLE));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(640, 400), BlockType::WALL_INDESTRUCTIBLE));

    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(400, 300), BlockType::WATER));
    levelBlocks.push_back(std::make_unique<Block>(sf::Vector2f(440, 300), BlockType::WATER));
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }

        handleStateSpecificEvents(event);
    }

    if (currentState == GameState::PLAYING) {
        handleGameplayInput();
    }
}

void Game::handleStateSpecificEvents(const sf::Event& event) {
    switch (currentState) {
    case GameState::MENU:
        handleMenuEvents(event);
        break;

    case GameState::PLAYING:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            currentState = GameState::PAUSED;
        }
        break;

    case GameState::PAUSED:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            currentState = GameState::PLAYING;
        }
        else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
            if (pauseResumeText.getGlobalBounds().contains(mousePos)) {
                currentState = GameState::PLAYING;
            }
            else if (pauseMenuText.getGlobalBounds().contains(mousePos)) {
                currentState = GameState::MENU;
            }
        }
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            if (pauseResumeText.getGlobalBounds().contains(mousePos)) {
                pauseResumeText.setFillColor(sf::Color::Yellow);
                pauseMenuText.setFillColor(sf::Color::White);
            }
            else if (pauseMenuText.getGlobalBounds().contains(mousePos)) {
                pauseResumeText.setFillColor(sf::Color::White);
                pauseMenuText.setFillColor(sf::Color::Yellow);
            }
            else {
                pauseResumeText.setFillColor(sf::Color::Yellow);
                pauseMenuText.setFillColor(sf::Color::White);
            }
        }
        break;

    case GameState::GAMEOVER:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            restartGame();
        }
        else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            std::vector<int> scores = loadHighScores();
            scores.push_back(score);
            std::sort(scores.rbegin(), scores.rend());
            if (scores.size() > 10) scores.resize(10);
            saveHighScores(scores);

            currentState = GameState::MENU;
        }
        break;

    case GameState::INSTRUCTIONS:
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {

            currentState = GameState::MENU;
        }
        break;
    }
}

void Game::handleMenuEvents(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Up && selectedOption > 0) {
            selectedOption--;
        }
        else if (event.key.code == sf::Keyboard::Down && selectedOption < static_cast<int>(menuOptions.size()) - 1) {
            selectedOption++;
        }
        else if (event.key.code == sf::Keyboard::Enter) {
            executeMenuOption();
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        for (size_t i = 0; i < menuOptions.size(); ++i) {
            if (menuOptions[i].getGlobalBounds().contains(mousePos)) {
                selectedOption = static_cast<int>(i);
                executeMenuOption();
                break;
            }
        }
    }

    else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        for (size_t i = 0; i < menuOptions.size(); ++i) {
            if (menuOptions[i].getGlobalBounds().contains(mousePos)) {
                selectedOption = static_cast<int>(i);
                break;
            }
        }
    }
}

void Game::executeMenuOption() {
    switch (selectedOption) {
    case 0:
        startGame();
        break;
    case 1:
        currentState = GameState::INSTRUCTIONS;
        break;
    case 2:
        window.close();
        break;
    }
}

void Game::handleGameplayInput() {
    if (showLevelNotification) return;

    if (!player || player->isDestroyed()) return;

    sf::Vector2f movement(0, 0);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) movement.y -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) movement.y += 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) movement.x -= 1;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) movement.x += 1;

    player->move(movement.x, movement.y);

    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f playerPos = player->getCenter();
    sf::Vector2f direction = mousePos - playerPos;

    if (direction.x != 0 || direction.y != 0) {
        float angle = atan2(direction.y, direction.x) * 180.0f / static_cast<float>(M_PI);
        player->setRotation(angle);
    }

    bool mouseLeftNow = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (mouseLeftNow && !mouseLeftPreviouslyPressed) {
        shootSound.play();
        player->attack(mousePos);
    }
    mouseLeftPreviouslyPressed = mouseLeftNow;
}

void Game::startGame() {
    mouseLeftPreviouslyPressed = true;
    currentState = GameState::PLAYING;
    score = 0;
    level = 1;
    gameOverText.setString("GAME OVER");
    centerText(gameOverText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f,
        static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f);
    setupLevel();
    gameTimer.restart();
}

void Game::restartGame() {
    std::vector<int> scores = loadHighScores();
    scores.push_back(score);
    std::sort(scores.rbegin(), scores.rend());
    if (scores.size() > 10) scores.resize(10);
    saveHighScores(scores);
    startGame();
}

void Game::update(float deltaTime) {
    if (player && !player->isDestroyed()) {
        player->update(deltaTime);
        player->updateMovement(deltaTime, levelBlocks);
    }

    for (auto& enemy : enemies) {
        if (enemy && !enemy->isDestroyed()) {
            enemy->update(deltaTime);
            enemy->updateMovement(deltaTime, levelBlocks);
        }
    }

    for (auto& bullet : bullets) {
        if (bullet && !bullet->isDestroyed()) {
            bullet->update(deltaTime);
        }
    }

    for (auto& block : levelBlocks) {
        if (block && !block->isDestroyed()) {
            block->update(deltaTime);
        }
    }

    spawnEnemies();

    checkCollisions();

    cleanupObjects();

    if (player && player->isDestroyed()) {
        currentState = GameState::GAMEOVER;
    }

    if (enemiesDefeated >= enemiesToDefeat) {
        nextLevel();
    }
}

void Game::spawnEnemies() {
    if (enemySpawnTimer.getElapsedTime().asSeconds() >= enemySpawnInterval) {
        if (enemies.size() < 3 && (enemiesDefeated + enemies.size()) < enemiesToDefeat) {
            sf::Vector2f spawnPos = getRandomSpawnPosition();
            auto enemy = std::make_unique<EnemyTank>(spawnPos, player.get(), &bullets);
            enemies.push_back(std::move(enemy));
        }
        enemySpawnTimer.restart();
    }
}

sf::Vector2f Game::getRandomSpawnPosition() {
    sf::Vector2f pos;
    bool validPosition = false;
    int attempts = 0;

    while (!validPosition && attempts < 100) {
        pos.x = 50 + static_cast<float>(rand() % (Constants::WINDOW_WIDTH - 100));
        pos.y = 50 + static_cast<float>(rand() % (Constants::WINDOW_HEIGHT - 100));

        validPosition = true;

        if (player) {
            sf::Vector2f playerPos = player->getCenter();
            float distance = sqrt(pow(pos.x - playerPos.x, 2) + pow(pos.y - playerPos.y, 2));
            if (distance < 200.0f) {
                validPosition = false;
            }
        }

        sf::FloatRect spawnArea(pos.x - 20, pos.y - 20, 40, 40);
        for (const auto& block : levelBlocks) {
            if (block && !block->isDestroyed() && block->isBlocking()) {
                if (spawnArea.intersects(block->getBounds())) {
                    validPosition = false;
                    break;
                }
            }
        }

        attempts++;
    }

    return pos;
}

void Game::checkCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet || bullet->isDestroyed()) continue;

        for (auto& block : levelBlocks) {
            if (!block || block->isDestroyed()) continue;

            if (bullet->getBounds().intersects(block->getBounds())) {
                if (block->isBlocking()) {
                    block->takeDamage(bullet->getDamage());
                    bullet->destroy();

                    if (block->isDestroyed()) {
                        score += 5;
                    }
                    break;
                }
            }
        }
    }

    for (auto& bullet : bullets) {
        if (!bullet || bullet->isDestroyed()) continue;

        if (bullet->isFromPlayer()) {
            for (auto& enemy : enemies) {
                if (!enemy || enemy->isDestroyed()) continue;

                if (bullet->getBounds().intersects(enemy->getBounds())) {
                    enemy->takeDamage(bullet->getDamage());
                    bullet->destroy();

                    if (enemy->isDestroyed()) {
                        score += 100;
                        enemiesDefeated++;
                    }
                    break;
                }
            }
        }
        else {
            if (player && !player->isDestroyed()) {
                if (bullet->getBounds().intersects(player->getBounds())) {
                    player->takeDamage(bullet->getDamage());
                    bullet->destroy();
                    break;
                }
            }
        }
    }

    if (player && !player->isDestroyed()) {
        for (auto& enemy : enemies) {
            if (!enemy || enemy->isDestroyed()) continue;

            if (player->getBounds().intersects(enemy->getBounds())) {
                sf::Vector2f separation = player->getCenter() - enemy->getCenter();
                float distance = sqrt(separation.x * separation.x + separation.y * separation.y);
                if (distance > 0) {
                    separation /= distance;
                    separation *= 2.0f;

                    sf::Vector2f playerPos = player->getCenter() + separation;
                    sf::Vector2f enemyPos = enemy->getCenter() - separation;

                    playerPos.x = std::max(20.0f, std::min(780.0f, playerPos.x));
                    playerPos.y = std::max(20.0f, std::min(580.0f, playerPos.y));
                    enemyPos.x = std::max(20.0f, std::min(780.0f, enemyPos.x));
                    enemyPos.y = std::max(20.0f, std::min(580.0f, enemyPos.y));

                    player->setPosition(playerPos);
                    enemy->setPosition(enemyPos);
                }
            }
        }
    }
}

void Game::cleanupObjects() {
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const std::unique_ptr<Bullet>& bullet) {
                return !bullet || bullet->isDestroyed();
            }),
        bullets.end()
    );

    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<EnemyTank>& enemy) {
                return !enemy || enemy->isDestroyed();
            }),
        enemies.end()
    );

    levelBlocks.erase(
        std::remove_if(levelBlocks.begin(), levelBlocks.end(),
            [](const std::unique_ptr<Block>& block) {
                return !block || block->isDestroyed();
            }),
        levelBlocks.end()
    );
}

void Game::nextLevel() {
    level++;
    score += 500;

    if (level > 5) {
        level--;
        showLevelNotification = false;
        gameOverText.setString("YOU WIN!");
        centerText(gameOverText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f,
            static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f);
        currentState = GameState::GAMEOVER;
        return;
    }

    enemySpawnInterval = std::max(1.0f, enemySpawnInterval - 0.2f);

    showLevelNotification = true;
    levelNotificationText.setFont(font);
    levelNotificationText.setCharacterSize(50);
    levelNotificationText.setFillColor(sf::Color::Red);

    levelHintText.setFont(font);
    levelHintText.setCharacterSize(24);
    levelHintText.setFillColor(sf::Color::Yellow);
    if (level == 2) {
        levelNotificationText.setString("LEVEL 2");
        levelHintText.setString("After each round, your tank gets bigger.");
    }
    else if (level == 3) {
        levelNotificationText.setString("LEVEL 3: QUICK DEATH");
        levelHintText.setString("Your enemy now moves faster.");
        levelNotificationDuration = 4.0f;
    }
    else if (level == 5) {
        levelNotificationText.setString("LEVEL 5: FINAL ROUND");
        levelHintText.setString("They get even more aggressive!");
        levelNotificationDuration = 4.0f;
    }
    else {
        levelNotificationText.setString("Level " + std::to_string(level) + "!");
        levelHintText.setString("");
        levelNotificationDuration = 2.0f;
    }
    centerText(levelNotificationText, Constants::WINDOW_WIDTH / 2.0f, Constants::WINDOW_HEIGHT / 2.0f - 30);
    centerText(levelHintText, Constants::WINDOW_WIDTH / 2.0f, Constants::WINDOW_HEIGHT / 2.0f + 40);
    levelNotificationTimer.restart();
}

void Game::render() {
    window.clear(sf::Color::Black);

    switch (currentState) {
    case GameState::MENU:
        renderMenu();
        break;

    case GameState::PLAYING:
        renderBackground();
        renderGame();
        if (showLevelNotification) {
            window.draw(levelNotificationText);
            if (!levelHintText.getString().isEmpty())
                window.draw(levelHintText);
        }
        break;

    case GameState::PAUSED:
        renderBackground();
        renderGame();
        renderPause();
        break;

    case GameState::GAMEOVER:
        renderBackground();
        renderGame();
        renderGameOver();
        break;

    case GameState::INSTRUCTIONS:
        renderInstructions();
        break;
    }

    window.display();
}

void Game::renderBackground() {
    if (background) {
        background->draw(window);
    }
}

void Game::renderMenu() {
    window.draw(titleText);

    for (size_t i = 0; i < menuOptions.size(); ++i) {
        if (i == static_cast<size_t>(selectedOption)) {
            menuOptions[i].setFillColor(sf::Color::Yellow);
        }
        else {
            menuOptions[i].setFillColor(sf::Color::White);
        }
        window.draw(menuOptions[i]);
    }

    sf::Text historyTitle;
    historyTitle.setFont(font);
    historyTitle.setString("HIGH SCORES:");
    historyTitle.setCharacterSize(24);
    historyTitle.setFillColor(sf::Color::Cyan);
    centerText(historyTitle, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, 450.0f);
    window.draw(historyTitle);

    std::vector<int> scores = loadHighScores();
    for (size_t i = 0; i < std::min(scores.size(), size_t(5)); ++i) {
        sf::Text scoreEntry;
        scoreEntry.setFont(font);
        scoreEntry.setString(std::to_string(i + 1) + ". " + std::to_string(scores[i]));
        scoreEntry.setCharacterSize(18);
        scoreEntry.setFillColor(sf::Color::White);
        centerText(scoreEntry, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, 480.0f + static_cast<float>(i) * 25.0f);
        window.draw(scoreEntry);
    }
}

void Game::renderGame() {
    for (const auto& block : levelBlocks) {
        if (block && !block->isDestroyed()) {
            block->draw(window);
        }
    }

    if (player && !player->isDestroyed()) {
        player->draw(window);
    }

    for (const auto& enemy : enemies) {
        if (enemy && !enemy->isDestroyed()) {
            enemy->draw(window);
        }
    }

    for (const auto& bullet : bullets) {
        if (bullet && !bullet->isDestroyed()) {
            bullet->draw(window);
        }
    }
    renderGameUI();
}

void Game::renderGameUI() {
    scoreText.setString("Score: " + std::to_string(score) + "  Level: " + std::to_string(level));
    window.draw(scoreText);

    if (player && !player->isDestroyed()) {
        sf::RectangleShape healthBg;
        healthBg.setSize(sf::Vector2f(210.0f, 40.0f));
        healthBg.setPosition(static_cast<float>(Constants::WINDOW_WIDTH) - 205, 1);
        healthBg.setFillColor(sf::Color::Red);
        window.draw(healthBg);

        sf::RectangleShape healthBar;
        float healthRatio = player->getHealth() / player->getMaxHealth();
        healthBar.setSize(sf::Vector2f(210.0f * healthRatio, 40.0f));
        healthBar.setPosition(static_cast<float>(Constants::WINDOW_WIDTH) - 205, 1);
        healthBar.setFillColor(sf::Color::Green);
        window.draw(healthBar);

        sf::Text healthText;
        healthText.setFont(font);
        healthText.setString("Health: " + std::to_string(static_cast<int>(player->getHealth())));
        healthText.setCharacterSize(16);
        healthText.setFillColor(sf::Color::White);
        healthText.setPosition(static_cast<float>(Constants::WINDOW_WIDTH) - 206, 42);
        window.draw(healthText);
    }

    sf::Text enemiesLeftText;
    enemiesLeftText.setFont(font);
    enemiesLeftText.setString("Enemies Left: " + std::to_string(enemiesToDefeat - enemiesDefeated));
    enemiesLeftText.setCharacterSize(16);
    enemiesLeftText.setFillColor(sf::Color::White);
    enemiesLeftText.setPosition(40, 40);
    window.draw(enemiesLeftText);
}

void Game::renderPause() {
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(static_cast<float>(Constants::WINDOW_WIDTH), static_cast<float>(Constants::WINDOW_HEIGHT)));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(overlay);

    sf::Text pauseText;
    pauseText.setFont(font);
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(48);
    pauseText.setFillColor(sf::Color::White);
    centerText(pauseText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f);
    window.draw(pauseText);

    window.draw(pauseResumeText);
    window.draw(pauseMenuText);
}

void Game::renderGameOver() {
    window.draw(gameOverText);

    sf::Text finalScoreText;
    finalScoreText.setFont(font);
    finalScoreText.setString("Final Score: " + std::to_string(score));
    finalScoreText.setCharacterSize(32);
    finalScoreText.setFillColor(sf::Color::Yellow);
    centerText(finalScoreText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f + 60);
    window.draw(finalScoreText);

    sf::Text levelText;
    levelText.setFont(font);
    levelText.setString("Level Reached: " + std::to_string(level));
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::Cyan);
    centerText(levelText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f + 100);
    window.draw(levelText);

    sf::Text restartText;
    restartText.setFont(font);
    restartText.setString("Press SPACE to restart or ESC for menu");
    restartText.setCharacterSize(18);
    restartText.setFillColor(sf::Color::White);
    centerText(restartText, static_cast<float>(Constants::WINDOW_WIDTH) / 2.0f, static_cast<float>(Constants::WINDOW_HEIGHT) / 2.0f + 140);
    window.draw(restartText);
}

void Game::renderInstructions() {
    for (const auto& text : instructionTexts) {
        window.draw(text);
    }
}

std::vector<int> Game::loadHighScores() {
    std::vector<int> scores;
    std::ifstream in(HIGH_SCORE_FILE);
    int s;
    while (in >> s) {
        scores.push_back(s);
    }
    std::sort(scores.rbegin(), scores.rend());
    if (scores.size() > 10) scores.resize(10);
    return scores;
}

void Game::saveHighScores(const std::vector<int>& scores) {
    std::ofstream out(HIGH_SCORE_FILE, std::ios::trunc);
    for (int s : scores) out << s << std::endl;
}