#include "obstacle.h"

Obstacle::Obstacle(const Vector& pos, const sf::FloatRect& hit)
    : position(pos), hitbox(hit) {
}

sf::FloatRect Obstacle::getGlobalHitbox() const {
    return sf::FloatRect(
            position.x + hitbox.left,
            position.y + hitbox.top,
            hitbox.width,
            hitbox.height
    );
}
