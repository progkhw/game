#include "entity.h"
#include "game.h"

#include <cmath>

Entity::Entity(float x, float y, float w, float h, const sf::Texture& texture)
    : position(x, y), direction(0.f, 0.f), speed(0.f), lookDirection(0.f, 0.f), size(w, h) {
        sprite.setTexture(texture);
        
        sprite.setTextureRect(sf::IntRect(0, 0, size.x, size.y));
        sprite.setOrigin(size.x / 2, size.y);
}

void Entity::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Entity::updateAnimation(int stepByX, int moveToLeft, int moveToRight, int moveToUp, int moveToDown, float time) {
    currentFrame += 0.005 * time;
    if (currentFrame > 3) {
        currentFrame -= 3;
    }
            
    int frameX = stepByX * int(currentFrame);
    if (std::abs(lookDirection.x) > std::abs(lookDirection.y)) {
        if (lookDirection.x < 0) {
            sprite.setTextureRect(sf::IntRect(frameX, moveToLeft, size.x, size.y));
        }
        else {
            sprite.setTextureRect(sf::IntRect(frameX, moveToRight, size.x, size.y));
        }
    }
    else {
        if (lookDirection.y < 0) {
            sprite.setTextureRect(sf::IntRect(frameX, moveToUp, size.x, size.y));
        }
        else {
            sprite.setTextureRect(sf::IntRect(frameX, moveToDown, size.x, size.y));
        }
    }
}



Vector Entity::getPosition() const {
    return position;
}

bool Entity::isColliding(float testX, float testY, const Game& game) {
    float left = testX + hitbox.left;
    float right = testX + hitbox.left + hitbox.width;
    float top = testY + hitbox.top;
    float bottom = testY + hitbox.top + hitbox.height;
    sf::FloatRect entityHitbox(left, top, hitbox.width, hitbox.height);
    const std::vector<Obstacle>& obstacles = game.getObstacles();

    int leftTile = std::floor(left / 32);
    int rightTile = std::floor(right / 32);
    int topTile = std::floor(top / 32);
    int bottomTile = std::floor(bottom / 32);

    for (int i = topTile; i <= bottomTile; ++i) {
        for (int j = leftTile; j <= rightTile; ++j) {
            if (i < 0 || i >= HEIGHT_MAP || j < 0 || j >= WIDTH_MAP) {
                return true;
            }

            if (game.isSolid(j, i)) {
                return true;
            }
        }
    }

    for (const Obstacle& obstacle : obstacles) {
        if (entityHitbox.intersects(obstacle.getGlobalHitbox())) {
            return true;
        }
    }


    return false;
}
