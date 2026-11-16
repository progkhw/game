#include "game.h"

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "entity.h"
#include "player.h"
#include "sheep.h"
#include "vector.h"

using json = nlohmann::json;

Game::Game() :
    window(sf::VideoMode(640, 480), "Отара"),
    player(640, 400, 96, 96, assets.get("hero")) {

    state = GameState::MainMenu;

    view.reset(sf::FloatRect(0, 0, 640, 480));
    hudView.reset(sf::FloatRect(0, 0, 640, 480));
    s_map.setTexture(assets.get("map"));
    font.loadFromFile("./assets/handgeschrieben.ttf");

    bool loading = loadLevel("./levels/level2.json");
    if (!loading) {
        std::cerr << "карта не загружена\n";
    }

    availableTime.setFont(font);
    availableTime.setCharacterSize(24);
    availableTime.setFillColor(sf::Color::Black);
    availableTime.setPosition(0, 0);

    countOfSheep.setFont(font);
    countOfSheep.setCharacterSize(24);
    countOfSheep.setFillColor(sf::Color::Black);
    countOfSheep.setPosition(40, 0);
}

bool Game::loadLevel(const std::string& path) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        std::cout << "file not opened!\n";
        return false;
    }

    obstacles.clear();

    json parser;
    inp >> parser;

    levelTime = parser["time"].get<float>();
    std::pair<int, int> playerSpawnCoor = parser["playerSpawn"].get<std::pair<int, int>>();
    std::vector<std::pair<int, int>> sheepSpawnCoor;
    for (const auto& sheepCoor : parser["sheepSpawn"]) {
        sheepSpawnCoor.push_back(sheepCoor.get<std::pair<int, int>>());
    }
    std::vector<std::string> mapSchema;
    for (const auto& mapString : parser["map"]) {
        mapSchema.push_back(mapString.get<std::string>());
    }

    if (mapSchema.empty() || sheepSpawnCoor.empty() || mapSchema[0].size() < 7) {
        return false;
    }

    mapHeight = mapSchema.size();
    mapWidth = mapSchema[0].size();
    for (const auto& i : mapSchema) {
        if (i.size() != mapWidth) {
            return false;
        }
    }

    player.setPosition(Vector(playerSpawnCoor.first * TILE_SIZE, playerSpawnCoor.second * TILE_SIZE));
    sheeps.clear();
    for (const auto& i : sheepSpawnCoor) {
        Sheep sheep(i.first * TILE_SIZE, i.second * TILE_SIZE, 32, 32, assets.get("sheep"));
        sheeps.push_back(sheep);
    }

    tileMap = mapSchema;

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            if (tileMap[i][j] == 'k') {
                Obstacle obj(Vector(j * TILE_SIZE, i * TILE_SIZE), sf::FloatRect(7.f, 6.f, 18.f, 18.f));
                obstacles.push_back(obj);
            }
            if (tileMap[i][j] == 't') {
                Obstacle obj(Vector(j * TILE_SIZE, i * TILE_SIZE), sf::FloatRect(5.f, 15.f, 24.f, 16.f));
                obstacles.push_back(obj);
            }
        }
    }

    return true;
}

bool Game::isSolid(int tx, int ty, bool canPassToZagon) const {
    if (tx < 0 || tx >= mapWidth || ty < 0 || ty >= mapHeight) {
        return true;
    }

    char c = tileMap[ty][tx];
    return c == '0' || (!canPassToZagon && c == 'i');
}

bool Game::isInTargetTile(float x, float y, char typeTile) const {
    int posTileX = std::floor(x / TILE_SIZE);
    int posTileY = std::floor(y / TILE_SIZE);

    if (posTileX < 0 || posTileX >= mapWidth || posTileY < 0 || posTileY >= mapHeight) {
        return false;
    }

    if (tileMap[posTileY][posTileX] == typeTile) {
        return true;
    }
    
    return false;
}

bool Game::isGameEnd() {
    int countSheepZagon = 0;
    for (const auto& sheep : sheeps) {
        if (sheep.getStatus() == StatusSheep::InZagon) {
            countSheepZagon++;
        }
    }
    countOfSheepInZagon = sheeps.size() - countSheepZagon;

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

std::pair<int, int> Game::getMapSize() const {
    return {mapWidth, mapHeight};
}

void Game::renderMainMenu() {
    window.setView(hudView);
    window.clear();
    
    sf::Sprite menuBack;
    sf::Text title1, title2, title3;

    menuBack.setTexture(assets.get("mainMenu"));

    title1.setFont(font);
    title1.setCharacterSize(40);
    title1.setFillColor(sf::Color::White);
    title1.setPosition(0, 0);
    title1.setString("Playing");

    title2.setFont(font);
    title2.setCharacterSize(40);
    title2.setFillColor(sf::Color::White);
    title2.setPosition(0, 0);
    title2.setString("Records");

    title3.setFont(font);
    title3.setCharacterSize(40);
    title3.setFillColor(sf::Color::White);
    title3.setPosition(0, 0);
    title3.setString("Exit");

    title1.setPosition(100, 30);
    title2.setPosition(100, 80);
    title3.setPosition(100, 130);
    menuBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        title1.setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        title2.setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        title3.setFillColor(sf::Color::Green);
    }

    window.draw(menuBack);

    window.draw(title1);
    window.draw(title2);
    window.draw(title3);

    window.display();
}

void Game::renderLevelSelect() {
    window.setView(hudView);
    window.clear();
    
    sf::Sprite levelBack;
    sf::Text level1, level2, level3, back;

    levelBack.setTexture(assets.get("levelBack"));

    level1.setFont(font);
    level1.setCharacterSize(40);
    level1.setFillColor(sf::Color::White);
    level1.setPosition(0, 0);
    level1.setString("Level 1");

    level2.setFont(font);
    level2.setCharacterSize(40);
    level2.setFillColor(sf::Color::White);
    level2.setPosition(0, 0);
    level2.setString("Level 2");

    level3.setFont(font);
    level3.setCharacterSize(40);
    level3.setFillColor(sf::Color::White);
    level3.setPosition(0, 0);
    level3.setString("Level 3");

    back.setFont(font);
    back.setCharacterSize(40);
    back.setFillColor(sf::Color::White);
    back.setPosition(0, 0);
    back.setString("Back");

    level1.setPosition(100, 30);
    level2.setPosition(100, 80);
    level3.setPosition(100, 130);
    back.setPosition(100, 180);
    levelBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        level1.setFillColor(sf::Color::Green);
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        level2.setFillColor(sf::Color::Green);
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        level3.setFillColor(sf::Color::Green);
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        back.setFillColor(sf::Color::Red);
    }

    window.draw(levelBack);

    window.draw(level1);
    window.draw(level2);
    window.draw(level3);
    window.draw(back);

    window.display();
}

void Game::renderPlaying() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
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
    window.draw(countOfSheep);

    window.display();
}

void Game::renderVictory() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
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
    
    sf::Sprite buttonToHome, buttonToRestart;
    buttonToHome.setTexture(assets.get("buttonsGame"));
    buttonToRestart.setTexture(assets.get("buttonsGame"));
    buttonToHome.setTextureRect(sf::IntRect(32, 0, 32, 32));
    buttonToRestart.setTextureRect(sf::IntRect(0, 0, 32, 32));
    buttonToHome.setPosition(280, 250);
    buttonToRestart.setPosition(328, 250);

    sf::Text victory;
    victory.setFont(font);
    victory.setCharacterSize(72);
    victory.setFillColor(sf::Color::Cyan);
    victory.setPosition(200, 155);
    victory.setString("Victory!");

    window.setView(hudView);
    window.draw(availableTime);
    window.draw(countOfSheep);
    window.draw(victory);
    window.draw(buttonToRestart);
    window.draw(buttonToHome);

    window.display();
}

void Game::renderDefeat() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
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
    window.draw(countOfSheep);

    window.display();
}

void Game::renderPause() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
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

    sf::Text pause;
    pause.setFont(font);
    pause.setCharacterSize(72);
    pause.setFillColor(sf::Color::Red);
    pause.setPosition(210, 206);
    pause.setString("PAUSE");

    window.setView(hudView);
    window.draw(availableTime);
    window.draw(countOfSheep);
    window.draw(pause);

    window.display();
}

void Game::render() {
    switch (state) {
        case GameState::MainMenu : {
            renderMainMenu();
            break;
        }
        case GameState::LevelSelect : {
            renderLevelSelect();
            break;
        }
        case GameState::Playing : {
            renderPlaying();
            break;
        }
        case GameState::Victory : {
            renderVictory();
            break;
        }
        case GameState::Pause : {
            renderPause();
            break;
        }
        default : {
            std::cout << "state is not defined\n";
            break;
        }
    }
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
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (state == GameState::MainMenu) {
                        handleMainMenuClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                    else if (state == GameState::LevelSelect) {
                        handleLevelSelectClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    if (state == GameState::Playing) {
                        state = GameState::Pause;
                    }
                    else if (state == GameState::Pause) {
                        state = GameState::Playing;
                    }
                }
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
        std::string cosiz = std::to_string(sheeps.size() - countOfSheepInZagon);
        cosiz = cosiz + " / " + std::to_string(sheeps.size());
        countOfSheep.setString(cosiz);

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
    
                if (tempX > (mapWidth * TILE_SIZE) - 320) {
                    tempX = (mapWidth * TILE_SIZE) - 320;
                }

                if (tempY < 240) {
                    tempY = 240;
                }

                if (tempY > (mapHeight * TILE_SIZE) - 240) {
                    tempY = (mapHeight * TILE_SIZE) - 240;
                }

                view.setCenter(tempX, tempY);

                window.setView(view);
            }
        }
        if (state == GameState::Exit) {
            window.close();
            break;
        }

        render();
    }
}

void Game::handleMainMenuClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        state = GameState::LevelSelect;
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        state = GameState::Records;
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        state = GameState::Exit;
    }
}

void Game::handleLevelSelectClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        if (loadLevel("./levels/level1.json")) {
            currentTime = 0.f;
            countOfSheepInZagon = 0;
            state = GameState::Playing;
        }
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (loadLevel("./levels/level2.json")) {
            currentTime = 0.f;
            countOfSheepInZagon = 0;
            state = GameState::Playing;
        }
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (loadLevel("./levels/level3.json")) {
            currentTime = 0.f;
            countOfSheepInZagon = 0;
            state = GameState::Playing;
        }
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        state = GameState::MainMenu;
    }

}
