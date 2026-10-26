#include "entity.h"
#include "game.h"

Sheep::Sheep(float x, float y, float w, float h, const Texture& name) : Entity(x, y, w, h, name) {
    hitbox = FloatRect(-12.f, -10.f, 24.f, 10.f);
}

void Sheep::update(float time, Game& game) {
    Player& player = game.getPlayer();
    Vector toSheep = position - player.getPosition();
    float dist = toSheep.length();

    if (dist < 224.f) {
        speed = toSheep.normalized() * 0.08f;
    }
    else {
        speed = Vector(0, 0);
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
