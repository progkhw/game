#include "entity.h"
#include "game.h"

#include <cmath>
#include "vector.h"
#include <optional>

// радиус обнаружения, радиус стада, радиус отталкивания
const float DETECTION_RADIUS = 224.f;
const float HERD_RADIUS = 192.f;
const float SEPARATION_RADIUS = 48.f;

const float EVASION = 64.f;

const float MAX_SPEED = 0.06f;
const float FRICTION = 0.9f;

const float W_FEAR = 5.0f;
const float W_ATTRACTION = 1.5f;
const float W_SEPARATION = 2.0f;

struct DirectionCandidate {
    float distance;
    float alignment;
    Vector direction;
};

// конструктор с параметрами для Sheep
Sheep::Sheep(float x, float y, float w, float h, const sf::Texture& name) : Entity(x, y, w, h, name) {
    hitbox = sf::FloatRect(-12.f, -10.f, 24.f, 10.f);
}

Vector Sheep::calculateFear(const Player& player) const {
    Vector dirFear(0.f, 0.f);

    Vector fromPlayer = position - player.getPosition();
    float distToPlayer = fromPlayer.length();

    if (distToPlayer < DETECTION_RADIUS && distToPlayer > 0.01f) {
        dirFear += fromPlayer.normalized() * W_FEAR * (1.f - distToPlayer / DETECTION_RADIUS);
    }
    
    return dirFear;
}

Vector Sheep::calculateAttraction(const std::vector<Sheep>& sheeps) const {
    Vector dirAttraction(0.f, 0.f);
    Vector centerHerd = position;
    int countSheeps = 1;

    for (const auto& sheep : sheeps) {
        if (&sheep == this) {
            continue;
        }

        float distance = (sheep.getPosition() - position).length();
        if (distance < HERD_RADIUS) {
            centerHerd += sheep.getPosition();
            countSheeps++;
        }
    }

    centerHerd /= countSheeps;
    
    Vector dirToHerd = centerHerd - position;
    if (dirToHerd.length() > 0.01f) {
        dirAttraction += dirToHerd.normalized() * W_ATTRACTION;
    }

    return dirAttraction;
}

Vector Sheep::calculateSeparation(const std::vector<Sheep>& sheeps) const {
    Vector dirSeparation(0.f, 0.f);

    for (const auto& sheep : sheeps) {
        if (&sheep == this) {
            continue;
        }

        Vector diff = position - sheep.getPosition();
        float distance = diff.length();
        if (distance > 0.01f && distance < SEPARATION_RADIUS) {
            dirSeparation += diff.normalized() * W_SEPARATION * (1.f - distance / SEPARATION_RADIUS);
        }
    }

    return dirSeparation;
}

std::vector<Vector> Sheep::getSupportDir() const {
    std::vector<Vector> supportDirs;

    for (int i = 0; i < 6; ++i) {
        Vector supportDir = direction;
        float curX = supportDir.x;
        float curY = supportDir.y;

        float angle = (i < 2) ? 30.f : 60.f;
        angle = (i > 3) ? angle + 30.f : angle;
        angle *= 3.14159265f / 180.f;

        if (i % 2 == 1) {
            angle = -angle;
        }

        supportDir.x = curX * std::cos(angle) - curY * std::sin(angle);
        supportDir.y = curX * std::sin(angle) + curY * std::cos(angle);
    
        supportDirs.push_back(supportDir.normalized());
    }

    return supportDirs;
}

float Sheep::isBarrierOnWay(const Vector& ray, float distance, const Game& game) {
    Vector section = ray * 5;
    Vector currentPos = position;
    float traveledDistance = 0;

    while (traveledDistance < distance) {
        if (section.length() > (distance - traveledDistance)) {
            section = ray * (distance - traveledDistance);
        }

        traveledDistance += section.length();
        currentPos += section;

        if (isColliding(currentPos.x, currentPos.y, game)) {
            return traveledDistance;
        }
    }

    return distance + 1;
}

Vector Sheep::getBestDir(const Game& game) {
    Vector mainDir = direction.normalized();
    Vector backDir = -mainDir;

    if (!(mainDir.length() > 0.f)) {
        return Vector(0.f, 0.f);
    }

    std::vector<Vector> supportDirs = getSupportDir();

    std::vector<DirectionCandidate> candidates;
    candidates.push_back({isBarrierOnWay(mainDir, EVASION, game), mainDir.dot(mainDir), mainDir});
    // candidates.push_back({isBarrierOnWay(backDir, EVASION, game), mainDir.dot(backDir), backDir});

    for (size_t i = 0; i < 6; ++i) {
        candidates.push_back({isBarrierOnWay(supportDirs[i], EVASION, game), mainDir.dot(supportDirs[i]), supportDirs[i]});
    }

    DirectionCandidate bestDir = candidates[0];
    if (bestDir.distance > EVASION / 2 + 10) {
        return bestDir.direction;
    }

    for (size_t i = 1; i < candidates.size(); ++i) {
        if (candidates[i].distance > bestDir.distance) {
            bestDir = candidates[i];
        }
    }
    
    return bestDir.direction;
}

// метод обновления
void Sheep::update(float time, Game& game) {
    Player& player = game.getPlayer();
    std::vector<Sheep>& sheeps = game.getSheeps();
    Vector dir(0.f, 0.f);

    Vector fear = calculateFear(player);
    if (fear.length() > 0.f) {
        Vector attraction = calculateAttraction(sheeps);
        Vector separation = calculateSeparation(sheeps);

        dir = fear + attraction + separation;
    }
    else {
        dir = fear;
    }

    if (dir.length() > 0.01f) {
        direction = dir.normalized();
        direction = getBestDir(game);
        speed = MAX_SPEED;
    }
    else {
        speed *= FRICTION;

        if (speed < 0.01f) {
            speed = 0.f;
        }
    }

    float nextX = position.x + direction.x * speed * time;
    if (!isColliding(nextX, position.y, game)) {
        position.x = nextX;
    }

    float nextY = position.y + direction.y * speed * time;
    if (!isColliding(position.x, nextY, game)) {
        position.y = nextY;
    }

    if (speed > 0.01f) {
        lookDirection = direction;
        updateAnimation(32, 64, 32, 96, 0, time);
    }
    else {
        currentFrame = 0.f;
        updateAnimation(32, 64, 32, 96, 0, 0.f);
    }

    sprite.setPosition(position.x, position.y);
}
