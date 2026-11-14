#include "game.h"

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include "entity.h"
#include "player.h"
#include "sheep.h"
#include "vector.h"

Game::Game() :
    window(sf::VideoMode(640, 480), "Отара"),
    player(640, 400, 96, 96, assets.get("hero")) {

    state = GameState::Playing;

    std::srand(std::time(nullptr));
    view.reset(sf::FloatRect(0, 0, 640, 480));
    hudView.reset(sf::FloatRect(0, 0, 640, 480));
    s_map.setTexture(assets.get("map"));
    font.loadFromFile("./assets/handgeschrieben.ttf");

    bool loading = loadLevel("./levels/map.txt");
    if (!loading) {
        std::cerr << "карта не загружена\n";
    }

    for (int i = 0; i < 3; ++i) {
        sheeps.push_back(Sheep((rand() % 750 + 50), (rand() % 900 + 50), 32, 32, assets.get("sheep")));
    }

    availableTime.setFont(font);
    availableTime.setCharacterSize(24);
    availableTime.setFillColor(sf::Color::Black);
    availableTime.setPosition(0, 0);
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

bool Game::isSolid(int tx, int ty, bool canPassToZagon) const {
    if (tx < 0 || tx >= WIDTH_MAP || ty < 0 || ty >= HEIGHT_MAP) {
        return true;
    }

    char c = tileMap[ty][tx];
    return c == '0' || (!canPassToZagon && c == 'i');
}

bool Game::isInTargetTile(float x, float y, char typeTile) const {
    int posTileX = std::floor(x / TILE_SIZE);
    int posTileY = std::floor(y / TILE_SIZE);

    if (posTileX < 0 || posTileX >= WIDTH_MAP || posTileY < 0 || posTileY >= HEIGHT_MAP) {
        return false;
    }

    if (tileMap[posTileY][posTileX] == typeTile) {
        return true;
    }
    
    return false;
}

bool Game::isGameEnd() const {
    int countSheepZagon = 0;
    for (const auto& sheep : sheeps) {
        if (sheep.getStatus() == StatusSheep::InZagon) {
            countSheepZagon++;
        }
    }

    if (countSheepZagon >= sheeps.size()) {
        return true;
    }

    return false;
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

void Game::render() {
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
            if (tileMap[i][j] == 'd') {
                s_map.setTextureRect(sf::IntRect(160, 0, 32, 32));
            }
            if (tileMap[i][j] == 'i') {
                s_map.setTextureRect(sf::IntRect(128, 0, 32, 32));
            }
                    
            s_map.setPosition(j * 32, i * 32);
                
            window.draw(s_map);
        }
    }

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    

    player.draw(window);

    window.setView(window.getDefaultView());

    window.setView(hudView);
    window.draw(availableTime);

    window.display();
}

void Game::run() {
    sf::Clock clock;

    while (window.isOpen()) {
        float timeSeconds = clock.getElapsedTime().asSeconds();
        clock.restart();
        float time = timeSeconds * 1250;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::Resized) {
                float viewRatio = static_cast<float>(event.size.width) / event.size.height;
                hudView.setViewport(sf::FloatRect(0, 0, 1, 1));

                if (viewRatio > 4.f / 3.f) {
                    float viewWidth = event.size.height * 4.f / 3.f;
                    float widthRatio = viewWidth / event.size.width;

                    view.setViewport(sf::FloatRect((1 - widthRatio) / 2, 0, widthRatio, 1));
                    hudView.setViewport(sf::FloatRect((1 - widthRatio) / 2, 0, widthRatio, 1));
                }

                if (viewRatio < 4.f / 3.f) {
                    float viewHeight = event.size.width * 3.f / 4.f;
                    float heightRatio = viewHeight / event.size.height;

                    view.setViewport(sf::FloatRect(0, (1 - heightRatio) / 2, 1, heightRatio));
                    hudView.setViewport(sf::FloatRect(0, (1 - heightRatio) / 2, 1, heightRatio));
                }
            }
        }

        availableTime.setString(std::to_string(static_cast<int>(std::ceil(levelTime - currentTime))));
        if (state == GameState::Playing) {
            currentTime += timeSeconds;

            if (isGameEnd()) {
                std::cout << "the game is end, you are victory!\n";
                state = GameState::Victory;
            }
            else if (levelTime - currentTime <= 0.f) {
                std::cout << "the game is defeat...\n";
                state = GameState::Defeat;
            }
            else if (state == GameState::Playing) {
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
            }
        }

        render();
    }
}
