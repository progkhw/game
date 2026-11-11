#pragma once

#include "vector.h"
#include <SFML/Graphics.hpp>

class Obstacle {
private:
    Vector position;
    sf::FloatRect hitbox;

public:
    Obstacle(const Vector& pos, const sf::FloatRect& hit);

    sf::FloatRect getGlobalHitbox() const;
};
