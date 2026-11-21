#include "player.h"

#include "entity.h"
#include "vector.h"
#include "game.h"

Player::Player(float x, float y, float w, float h, const sf::Texture& name) : Entity(x, y, w, h, name) {    
    hitbox = sf::FloatRect(-16.f, -20.f, 32.f, 20.f);
}

void Player::setDirection(Vector dir) {
    direction = dir.normalized();

    if (direction.length() > 0.f) {
        lookDirection = direction;
    }

    speed = direction.length() > 0.f ? 0.1f : 0.f;
}

void Player::update(float time, Game& game) {
    if (speed > 0) {
        updateAnimation(96, 96, 192, 288, 0, time);
    }

    float coef = 1;
    if (game.isInTargetTile(position.x, position.y, 'w')) {
        coef = 0.5; 
    }

    float nextX = position.x + direction.x * speed * time * coef;
    if (!isColliding(nextX, position.y, game)) {
        position.x = nextX;
    }

    float nextY = position.y + direction.y * speed * time * coef;
    if (!isColliding(position.x, nextY, game)) {
        position.y = nextY;
    }

    sprite.setPosition(position.x, position.y);
}

bool Player::shouldWalk() {
    if (speed > 0.f) {
        return true;
    }

    return false;
}
