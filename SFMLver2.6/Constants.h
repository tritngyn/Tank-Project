#pragma once

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Constants {
    const unsigned WINDOW_WIDTH = 800;
    const unsigned WINDOW_HEIGHT = 600;
    const unsigned BLOCK_SIZE = 40;

    const float PLAYER_SPEED = 200.0f;
    const float ENEMY_SPEED = 120.0f;
    const float BULLET_SPEED = 600.0f;
    const float ENEMY_BULLET_SPEED = 400.0f;

    const float PLAYER_HEALTH = 100.0f;
    const float ENEMY_HEALTH = 100.0f;
    const float WALL_HEALTH = 1000.0f;
    const float DESTRUCTIBLE_HEALTH = 50.0f;

    const float PLAYER_BULLET_DAMAGE = 25.0f;
    const float ENEMY_BULLET_DAMAGE = 20.0f;

    const float PLAYER_ATTACK_COOLDOWN = 0.3f;
    const float ENEMY_ATTACK_COOLDOWN = 1.5f;

    const float ENEMY_SPAWN_INTERVAL = 3.0f;
    const float AI_UPDATE_INTERVAL = 2.0f;
    const float ENEMY_DETECTION_RANGE = 300.0f;
    const float ENEMY_MIN_DISTANCE = 100.0f;
    const float ENEMY_MAX_DISTANCE = 150.0f;
}