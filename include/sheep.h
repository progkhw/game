#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"
#include "player.h"
#include "vector.h"

class Game;

enum class StatusSheep {
    Free,
    EnteringZagon,
    InZagon
};

class Sheep final : public Entity {
private:
    StatusSheep status = StatusSheep::Free;
    float timer = 0.f;

public:
    Sheep(float x, float y, float w, float h, const sf::Texture& name);

    void update(float time, Game& game) override;

    Vector calculateFear(const Player& player) const;
    Vector calculateAttraction(const std::vector<Sheep>& sheeps) const;
    Vector calculateSeparation(const std::vector<Sheep>& sheeps) const;
    std::vector<Vector> getSupportDir() const;
    float isBarrierOnWay(const Vector& ray, float distance, const Game& game);
    Vector getBestDir(const Game& game);

    StatusSheep getStatus() const;
    void updateZagonStatus(const Game& game, float time);
};
