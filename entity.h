#pragma once

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>
#include "vector.h"
#include <optional>

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

class Player final : public Entity {
private:

public:
    Player(float x, float y, float w, float h, const sf::Texture& name);
   
    void setDirection(Vector dir);
    void update(float time, Game& game) override;
};

class Sheep final : public Entity {
private:
    bool isTrapped = false;

public:
    Sheep(float x, float y, float w, float h, const sf::Texture& name);

    void update(float time, Game& game) override;

    Vector calculateFear(const Player& player) const;
    Vector calculateAttraction(const std::vector<Sheep>& sheeps) const;
    Vector calculateSeparation(const std::vector<Sheep>& sheeps) const;
    std::vector<Vector> getSupportDir() const;
    float isBarrierOnWay(const Vector& ray, float distance, const Game& game);
    Vector getBestDir(const Game& game);
    //Vector calculatePushingWall(const std::array<std::string, HEIGHT_MAP>& map) const;
};
