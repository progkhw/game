#include "entity.h"
#include "vector.h"
#include "map.h"
#include "game.h"
#include <string>

using namespace sf;

Player::Player(float x, float y, float w, float h, const Texture& name) : Entity(x, y, w, h, name), lastDirection(0.f, 0.f) {    
    hitbox = FloatRect(-16.f, -16.f, 32.f, 16.f);
}

void Player::setDirection(Vector dir) {
    lastDirection = dir;
    float playerSpeed = 0.1;
    speed = dir.normalized() * playerSpeed;
}

void Player::update(float time, Game& game) {
    if (lastDirection.length() > 0) {
        currentFrame += 0.005 * time;
        if (currentFrame > 3) {
            currentFrame -= 3;
        }
        
        int frameX = 64 * int(currentFrame);
        if (lastDirection.y < 0) {
            sprite.setTextureRect(IntRect(frameX, 0, 64, 96));
        }
        if (lastDirection.y > 0) {
            sprite.setTextureRect(IntRect(frameX, 288, 64, 96));
        }
        if (lastDirection.x < 0) {
            sprite.setTextureRect(IntRect(frameX, 192, 64, 96));
        }
        if (lastDirection.x > 0) {
            sprite.setTextureRect(IntRect(frameX, 96, 64, 96));
        }
    }

    float nextX = position.x + speed.x * time;
    if (!isColliding(nextX, position.y)) {
        position.x = nextX;
    }

    float nextY = position.y + speed.y * time;
    if (!isColliding(position.x, nextY)) {
        position.y = nextY;
    }

    sprite.setPosition(position.x, position.y);
}
