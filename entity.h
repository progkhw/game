#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include "vector.h"
#include "map.h"

class Game;

using namespace sf;

class Entity {
protected:
    Vector position;
    Vector speed;

    float currentFrame = 0.f;

    Vector size;
    FloatRect hitbox;

    Sprite sprite;

public:
    Entity(float x, float y, float w, float h, const Texture& texture)
        : position(x, y), size(w, h) {
            sprite.setTexture(texture);

            sprite.setTextureRect(IntRect(0, 0, size.x, size.y));
            sprite.setOrigin(size.x / 2, size.y);
    }

    virtual ~Entity() = default;

    virtual void update(float time, Game& game) = 0;
    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    Vector getPosition() const {
        return position;
    }

    bool isColliding(float testX, float testY) {
        float leftTile = (testX + hitbox.left) / 32;
        float rightTile = (testX + hitbox.left + hitbox.width) / 32;
        float topTile = (testY + hitbox.top) / 32;
        float bottomTile = (testY + hitbox.top + hitbox.height) / 32;

        for (int i = topTile; i <= bottomTile; ++i) {
            for (int j = leftTile; j <= rightTile; ++j) {
                if (i < 0 || i >= HEIGHT_MAP || j < 0 || j >= WIDTH_MAP) {
                    return true;
                }

                if (TileMap[i][j] == '0') {
                    return true;
                }
            }
        }

        return false;
    }
};

class Player final : public Entity {
private:
    Vector lastDirection;

public:
    Player(float x, float y, float w, float h, const Texture& name);
   
    void setDirection(Vector dir);
    void update(float time, Game& game) override;
};

class Sheep final : public Entity {
private:
    bool is_trapped = false;
    Vector lastDirection;

public:
    Sheep(float x, float y, float w, float h, const Texture& name);

    void update(float time, Game& game) override;
};
