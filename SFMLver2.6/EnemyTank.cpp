#include "EnemyTank.h"
#include "PlayerTank.h"
#include "Block.h"
#include <cmath>
#include <cstdlib>
#include <vector>

float quantizeAngle(float angleDegrees);

EnemyTank::EnemyTank(sf::Vector2f pos, PlayerTank* playerRef, std::vector<std::unique_ptr<Bullet>>* bullets)
    : BaseTank(pos, Constants::ENEMY_SPEED, sf::Color::Red, bullets),
    player(playerRef), aiTimer(0.0f) {

    if (texture.loadFromFile("assets/EnemyTank.png")) {
        useTexture = true;
        sprite.setTexture(texture);
        sprite.setScale(0.07f, 0.07f);
        sprite.setOrigin(static_cast<float>(texture.getSize().x) / 2.0f,
            static_cast<float>(texture.getSize().y) / 2.0f);
        sprite.setPosition(pos);
    }

    maxAttackCooldown = Constants::ENEMY_ATTACK_COOLDOWN;
    targetDirection = sf::Vector2f(0, 0);
}

void EnemyTank::update(float deltaTime) {
    if (destroyed || !player || player->isDestroyed()) return;

    aiTimer += deltaTime;

    sf::Vector2f playerPos = player->getCenter();
    sf::Vector2f direction = playerPos - position;
    float distance = sqrt(direction.x * direction.x + direction.y * direction.y);

    if (distance > 0) {
        float angleRad = atan2(direction.y, direction.x);
        float angleDeg = angleRad * 180.0f / static_cast<float>(M_PI);

        setRotation(angleDeg);

        float quantizedAngle = quantizeAngle(angleDeg);
        float quantizedRad = quantizedAngle * static_cast<float>(M_PI) / 180.0f;
        sf::Vector2f quantizedDir(cos(quantizedRad), sin(quantizedRad));

        if (distance > Constants::ENEMY_MAX_DISTANCE) {
            targetDirection = quantizedDir;
        }
        else if (distance < Constants::ENEMY_MIN_DISTANCE) {
            targetDirection = -quantizedDir;
        }
        else {
            targetDirection = sf::Vector2f(0, 0);
        }
    }
    else {
        targetDirection = sf::Vector2f(0, 0);
    }

    velocity = targetDirection * speed;

    if (aiTimer >= Constants::AI_UPDATE_INTERVAL && distance < Constants::ENEMY_DETECTION_RANGE) {
        attack(playerPos);
        aiTimer = 0.0f;
    }

    BaseTank::update(deltaTime);

    if (useTexture) {
        sprite.setPosition(position);
        sprite.setRotation(rotation + 90.f);
    }
}


void EnemyTank::updateMovement(float deltaTime, const std::vector<std::unique_ptr<Block>>& blocks) {
    if (destroyed) return;

    sf::Vector2f newPos = position + velocity * deltaTime;

    newPos.x = std::max(20.0f, std::min(780.0f, newPos.x));
    newPos.y = std::max(20.0f, std::min(580.0f, newPos.y));

    if (canMoveTo(newPos, blocks)) {
        setPosition(newPos);
    } else {
        float angleRad = atan2(targetDirection.y, targetDirection.x);
        float angleDeg = angleRad * 180.0f / static_cast<float>(M_PI);

        static const std::vector<float> allowedAngles = {30, 45, -45, 90, -90, 135, -135, 0, 180, -180};

        int bestIdx = 0;
        float minDiff = std::abs(angleDeg - allowedAngles[0]);
        for (int i = 1; i < allowedAngles.size(); ++i) {
            float diff = std::abs(angleDeg - allowedAngles[i]);
            if (diff < minDiff) {
                minDiff = diff;
                bestIdx = i;
            }
        }

        for (int offset = 1; offset < allowedAngles.size(); ++offset) {
            for (int sign = -1; sign <= 1; sign += 2) {
                int idx = (bestIdx + sign * offset + allowedAngles.size()) % allowedAngles.size();
                float tryAngle = allowedAngles[idx];
                float tryRad = tryAngle * static_cast<float>(M_PI) / 180.0f;
                sf::Vector2f tryDir(cos(tryRad), sin(tryRad));
                sf::Vector2f tryPos = position + tryDir * speed * deltaTime;
                tryPos.x = std::max(20.0f, std::min(780.0f, tryPos.x));
                tryPos.y = std::max(20.0f, std::min(580.0f, tryPos.y));
                if (canMoveTo(tryPos, blocks)) {
                    targetDirection = tryDir;
                    velocity = targetDirection * speed;
                    setPosition(tryPos);
                    return;
                }
            }
        }
        velocity = sf::Vector2f(0, 0);
    }
}

void EnemyTank::createBullet(sf::Vector2f target) {
    sf::Vector2f direction = target - position;
    auto bullet = std::make_unique<Bullet>(position, direction, 5.0f,
        sf::Color::Cyan, Constants::ENEMY_BULLET_SPEED,
        Constants::ENEMY_BULLET_DAMAGE, false);
    bulletContainer->push_back(std::move(bullet));
}

float quantizeAngle(float angleDegrees) {
    static const std::vector<float> allowedAngles = {30, 45, -45, 90, -90, 135, -135, 0, 180, -180};
    float closest = allowedAngles[0];
    float minDiff = std::abs(angleDegrees - closest);
    for (float a : allowedAngles) {
        float diff = std::abs(angleDegrees - a);
        if (diff < minDiff) {
            minDiff = diff;
            closest = a;
        }
    }
    return closest;
}

void EnemyTank::draw(sf::RenderWindow& window) {
    if (!destroyed) {
        if (useTexture) {
            window.draw(sprite);
        }
        else {
            window.draw(body);
        }
        drawHealthBar(window);
    }
}