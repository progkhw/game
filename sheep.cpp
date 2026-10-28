#include "entity.h"
#include "game.h"

// радиус обнаружения, радиус стада, радиус отталкивания
const float DETECTION_RADIUS = 224.f;
const float HERD_RADIUS = 192.f;
const float SEPARATION_RADIUS = 48.f;

const float MAX_SPEED = 0.06f;
const float FRICTION = 0.9f;

const float W_FEAR = 5.0f;
const float W_ATTRACTION = 1.5f;
const float W_SEPARATION = 2.0f;

// конструктор с параметрами для Sheep
Sheep::Sheep(float x, float y, float w, float h, const Texture& name) : Entity(x, y, w, h, name) {
    hitbox = FloatRect(-12.f, -10.f, 24.f, 10.f);
}

// метод обновления
void Sheep::update(float time, Game& game) {
    Player& player = game.getPlayer();
    std::vector<Sheep>& sheeps = game.getSheeps();

    Vector fromPlayer = position - player.getPosition();
    float distToPlayer = fromPlayer.length();

    Vector dir(0.f, 0.f);

    if (distToPlayer < DETECTION_RADIUS && distToPlayer > 0.01f) {
        dir += fromPlayer.normalized() * W_FEAR * (1.f - distToPlayer / DETECTION_RADIUS);

        Vector centerHerd = position;
        int countSheeps = 1;
    
        for (const auto& i : sheeps) {
            if (&i == this) {
                continue;
            }

            if ((i.getPosition() - position).length() < HERD_RADIUS) {
                centerHerd += i.getPosition();
                countSheeps += 1;
            }
        }
        centerHerd /= countSheeps;

        Vector toCenter = centerHerd - position;
        if (toCenter.length() > 0.01f) {
            dir += toCenter.normalized() * W_ATTRACTION;
        }

        for (const auto& i: sheeps) {
            if (&i == this) {
                continue;
            }

            Vector diff = position - i.getPosition();
            float d = diff.length();
            if (d > 0.01f && d < SEPARATION_RADIUS) {
                dir += diff.normalized() * W_SEPARATION * (1.f - d / SEPARATION_RADIUS);
            }
        }
        
    }

    if (dir.length() > 0.01f) {
        speed = dir.normalized() * MAX_SPEED;
    }
    else {
        speed *= FRICTION;
        if (speed.length() < 0.01f) {
            speed = Vector(0.f, 0.f);
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

    if (speed.length() > 0) {
        lastDirection = speed.normalized();
        currentFrame += 0.005 * time;
        if (currentFrame > 3) {
            currentFrame -= 3;
        }
            
        int frameX = 32 * int(currentFrame);
        if (std::abs(lastDirection.x) > std::abs(lastDirection.y)) {
            if (lastDirection.x < 0) {
                sprite.setTextureRect(IntRect(frameX, 64, 32, 32));
            }
            else {
                sprite.setTextureRect(IntRect(frameX, 32, 32, 32));
            }
        }
        else {
            if (lastDirection.y < 0) {
                sprite.setTextureRect(IntRect(frameX, 96, 32, 32));
            }
            else {
                sprite.setTextureRect(IntRect(frameX, 0, 32, 32));
            }
        }
    }

    sprite.setPosition(position.x, position.y);
}
