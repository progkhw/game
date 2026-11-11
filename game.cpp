#include "game.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "entity.h"
#include "vector.h"

Game::Game() :
    window(sf::VideoMode(640, 480), "Отара"),
    player(640, 400, 96, 96, assets.get("hero")) {

    view.reset(sf::FloatRect(0, 0, 640, 480));
    s_map.setTexture(assets.get("map"));

    bool loading = loadLevel("/home/progkhw/study/game/map.txt");
    if (!loading) {
        std::cerr << "карта не загружена\n";
    }

    for (int i = 0; i < 5; ++i) {
        sheeps.push_back(Sheep(200 + i * 32, 250, 32, 32, assets.get("sheep")));
    }
}

bool Game::loadLevel(const std::string& path) {
    std::ifstream inf(path);
    if (!inf) {
        return false;
    }

    obstacles.clear();

    std::string line;
    for (int i = 0; i < HEIGHT_MAP; ++i) {
        if (!std::getline(inf, line)) {
            return false;
        }

        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        line.resize(WIDTH_MAP, ' ');
        tileMap[i] = line;

        for (int j = 0; j < WIDTH_MAP; ++j) {
            if (tileMap[i][j] == 'k') {
                Obstacle obj(Vector(j * TILE_SIZE, i * TILE_SIZE), sf::FloatRect(7.f, 6.f, 18.f, 18.f));
                obstacles.push_back(obj);
            }
        }
    }

    return true;
}

bool Game::isSolid(int tx, int ty) const {
    if (tx < 0 || tx >= WIDTH_MAP || ty < 0 || ty >= HEIGHT_MAP) {
        return true;
    }

    char c = tileMap[ty][tx];
    return c == '0';
}

Player& Game::getPlayer() {
    return player;
}

std::vector<Sheep>& Game::getSheeps() {
    return sheeps;
}

const std::vector<Obstacle>& Game::getObstacles() const {
    return obstacles;
}

/*std::array<std::string, HEIGHT_MAP>& getMap() {
    return tileMap;
}*/

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 800;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        Vector direction;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            direction.y -= 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            direction.y += 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            direction.x -= 1.f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            direction.x += 1.f;
        }

        player.setDirection(direction);

        player.update(time, *this);
        for (int i = 0; i < sheeps.size(); ++i) {
            sheeps[i].update(time, *this);
        }

        float tempX = player.getPosition().x;
        float tempY = player.getPosition().y;
        if (tempX < 320) {
            tempX = 320;
        }
    
        if (tempX > 1056) {
            tempX = 1056;
        }

        if (tempY < 240) {
            tempY = 240;
        }

        if (tempY > 784) {
            tempY = 784;
        }

        view.setCenter(tempX, tempY);
        window.setView(view);
        window.clear();


        for (int i = 0; i < HEIGHT_MAP; ++i) {
            for (int j = 0; j < WIDTH_MAP; ++j) {
                if (tileMap[i][j] == ' ') {
                    s_map.setTextureRect(sf::IntRect(32, 0, 32, 32));
                }
                if (tileMap[i][j] == 's') {
                    s_map.setTextureRect(sf::IntRect(64, 0, 32, 32));
                }
                if (tileMap[i][j] == '0') {
                    s_map.setTextureRect(sf::IntRect(0, 0, 32, 32));
                }
                if (tileMap[i][j] == 'k') {
                    s_map.setTextureRect(sf::IntRect(96, 0, 32, 32));
                }

                s_map.setPosition(j * 32, i * 32);
                
                window.draw(s_map);
            }
        }

        for (int i = 0; i < sheeps.size(); ++i) {
            sheeps[i].draw(window);
        }
        player.draw(window);
        window.display();
    }
}
