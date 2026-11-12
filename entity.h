#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include "vector.h"

class Game;

class Entity {
protected:
    Vector position;
    Vector direction;
    float speed;
    Vector lookDirection;

    float currentFrame = 0.f;
    Vector size;
    sf::FloatRect hitbox;
    sf::Sprite sprite;

public:
    Entity(float x, float y, float w, float h, const sf::Texture& texture);
    virtual ~Entity() = default;

    virtual void update(float time, Game& game) = 0;
    void draw(sf::RenderWindow& window);
    void updateAnimation(int stepByX, int moveToLeft, int moveToRight, int moveToUp, int moveToDown, float time);

    Vector getPosition() const;
    bool isColliding(float testX, float testY, const Game& game, bool canPassToZagon = false);
};
