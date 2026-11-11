#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"
#include "vector.h"
#include "obstacle.h"
#include <array>
#include <vector>
#include <string>
#include <map>

const int HEIGHT_MAP = 32;
const int WIDTH_MAP = 43;
const float TILE_SIZE = 32.f;

class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;

public:
    AssetManager() {
        textures["hero"].loadFromFile("/home/progkhw/study/game/icons/hero.png");
        textures["map"].loadFromFile("/home/progkhw/study/game/icons/new_map.png");
        textures["sheep"].loadFromFile("/home/progkhw/study/game/icons/sheep_hi.png");
    }

    const sf::Texture& get(const std::string& name) {
        return textures[name];
    }
};

class Game {
private:
    sf::RenderWindow window;
    AssetManager assets;
    Player player;
    std::vector<Sheep> sheeps;
    std::vector<Obstacle> obstacles;
    sf::Sprite s_map;
    sf::View view;
    std::array<std::string, HEIGHT_MAP> tileMap;

public:
    Game();

    bool loadLevel(const std::string& path);
    bool isSolid(int tx, int ty) const;

    Player& getPlayer();
    std::vector<Sheep>& getSheeps();
    const std::vector<Obstacle>& getObstacles() const;
    std::array<std::string, HEIGHT_MAP>& getMap();

    void run();
};
