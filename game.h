#pragma once

#include <SFML/Graphics.hpp>
#include "entity.h"
#include "vector.h"
#include "map.h"

using namespace sf;

class AssetManager {
private:
    std::map<std::string, Texture> textures;

public:
    AssetManager() {
        textures["hero"].loadFromFile("icons/hero_new_2.png");
        textures["map"].loadFromFile("icons/Sprite-0001.png");
        textures["sheep"].loadFromFile("icons/hello.png");
    }

    const Texture& get(const std::string& name) {
        return textures[name];
    }
};

class Game {
private:
    RenderWindow window;
    AssetManager assets;
    Player player;
    std::vector<Sheep> sheeps;
    Sprite s_map;
    View view;

public:
    Game();

    Player& getPlayer();
    std::vector<Sheep>& getSheeps();
    void run();
};
