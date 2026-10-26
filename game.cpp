#include "game.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include "entity.h"
#include "vector.h"
#include "map.h"

Game::Game() :
    window(VideoMode(640, 480), "Отара"),
    player(640, 400, 64, 96, assets.get("hero")) {

    view.reset(FloatRect(0, 0, 640, 480));
    s_map.setTexture(assets.get("map"));

    for (int i = 0; i < 5; ++i) {
        sheeps.push_back(Sheep(200 + i * 32, 250, 32, 32, assets.get("sheep")));
    }
}

Player& Game::getPlayer() {
    return player;
}

std::vector<Sheep>& Game::getSheeps() {
    return sheeps;
}

void Game::run() {
    Clock clock;

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asMicroseconds();
        clock.restart();
        time = time / 800;

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        Vector direction;
        if (Keyboard::isKeyPressed(Keyboard::W)) {
            direction.y -= 1.f;
        }
        if (Keyboard::isKeyPressed(Keyboard::S)) {
            direction.y += 1.f;
        }
        if (Keyboard::isKeyPressed(Keyboard::A)) {
            direction.x -= 1.f;
        }
        if (Keyboard::isKeyPressed(Keyboard::D)) {
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
    
        if (tempX > 960) {
            tempX = 960;
        }

        if (tempY < 240) {
            tempY = 240;
        }

        if (tempY > 560) {
            tempY = 560;
        }

        view.setCenter(tempX, tempY);
        window.setView(view);
        window.clear();

        for (int i = 0; i < HEIGHT_MAP; ++i) {
            for (int j = 0; j < WIDTH_MAP; ++j) {
                if (TileMap[i][j] == ' ') {
                    s_map.setTextureRect(IntRect(32, 0, 32, 32));
                }
                if (TileMap[i][j] == 's') {
                    s_map.setTextureRect(IntRect(64, 0, 32, 32));
                }
                if (TileMap[i][j] == '0') {
                    s_map.setTextureRect(IntRect(0, 0, 32, 32));
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
