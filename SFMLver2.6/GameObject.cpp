#include "GameObject.h"

GameObject::GameObject(sf::Vector2f pos, float hp)
    : position(pos), destroyed(false), health(hp), maxHealth(hp) {
}

sf::Vector2f GameObject::getCenter() const {
    return position;
}

bool GameObject::isDestroyed() const {
    return destroyed;
}

float GameObject::getHealth() const {
    return health;
}

float GameObject::getMaxHealth() const {
    return maxHealth;
}

void GameObject::destroy() {
    destroyed = true;
}

void GameObject::takeDamage(float damage) {
    health -= damage;
    if (health <= 0) {
        destroyed = true;
    }
}