#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"
#include "vector.h"

class Game;

class Player final : public Entity {
private:

public:
    Player(float x, float y, float w, float h, const sf::Texture& name);
   
    void setDirection(Vector dir);
    void update(float time, Game& game) override;
};
