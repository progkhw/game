#include "game.h"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    player(640, 400, 64, 96, assets.getTexture("hero")) {

    state = GameState::MainMenu;

    assets.getMusic("mainMusic").setLoop(true);
    assets.getMusic("mainMusic").play();

    buttonTouching.setBuffer(assets.getSound("touchButton"));
    levelCompleting.setBuffer(assets.getSound("levelCompleting"));
    levelFailing.setBuffer(assets.getSound("levelFailing"));
    hotiba.setBuffer(assets.getSound("walking"));
    bleatings.reserve(3);
    for (size_t i = 0; i < 3; ++i) {
        std::string currentBleating = "bleating" + std::to_string(i + 1);
        bleatings.emplace_back();
        bleatings.back().setBuffer(assets.getSound(currentBleating));
    }

    levelCompleting.setVolume(50);
    levelFailing.setVolume(50);
    hotiba.setVolume(100);
    buttonTouching.setVolume(70);
    assets.getMusic("mainMusic").setVolume(15);
    for (size_t i = 0; i < 3; ++i) {
        bleatings[i].setVolume(20);
    }

    view.reset(sf::FloatRect(0, 0, 640, 480));
    hudView.reset(sf::FloatRect(0, 0, 640, 480));
    s_map.setTexture(assets.getTexture("map"));
    zagon.setTexture(assets.getTexture("map"));
    zagon.setTextureRect(sf::IntRect(0, 32, 160, 160));
    font = &assets.getFont("handgeschrieben");

    bool loading = loadLevel("./levels/level1.json");
    if (!loading) {
        std::cerr << "карта не загружена\n";
    }

    availableTime.setFont(*font);
    availableTime.setCharacterSize(24);
    availableTime.setFillColor(sf::Color{0, 204, 204});
    availableTime.setPosition(0, 0);

    countOfSheep.setFont(*font);
    countOfSheep.setCharacterSize(24);
    countOfSheep.setFillColor(sf::Color{0, 204, 204});
    countOfSheep.setPosition(60, 0);

    currentLevel.setFont(*font);
    currentLevel.setCharacterSize(24);
    currentLevel.setFillColor(sf::Color{0, 204, 204});
    currentLevel.setPosition(140, 0);
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
    zagonSpawn = parser["zagonSpawn"].get<std::pair<float, float>>();
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
    zagon.setPosition(zagonSpawn.first * 32, zagonSpawn.second * 32);
    sheeps.clear();
    for (const auto& i : sheepSpawnCoor) {
        Sheep sheep(i.first * TILE_SIZE, i.second * TILE_SIZE, 32, 32, assets.getTexture("sheep"));
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
    return c == '0' || c == 'l' || (!canPassToZagon && c == 'i');
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
    sf::Text title1, title2, title3, title4;

    menuBack.setTexture(assets.getTexture("mainMenu"));

    title1.setFont(*font);
    title1.setCharacterSize(40);
    title1.setFillColor(sf::Color::White);
    title1.setPosition(0, 0);
    title1.setString("Playing");

    title2.setFont(*font);
    title2.setCharacterSize(40);
    title2.setFillColor(sf::Color::White);
    title2.setPosition(0, 0);
    title2.setString("Records");

    title3.setFont(*font);
    title3.setCharacterSize(40);
    title3.setFillColor(sf::Color::White);
    title3.setPosition(0, 0);
    title3.setString("Settings");

    title4.setFont(*font);
    title4.setCharacterSize(40);
    title4.setFillColor(sf::Color::White);
    title4.setPosition(0, 0);
    title4.setString("Exit");

    title1.setPosition(100, 30);
    title2.setPosition(100, 80);
    title3.setPosition(100, 130);
    title4.setPosition(100, 180);
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
        title3.setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        title4.setFillColor(sf::Color::Red);
    }

    window.draw(menuBack);

    window.draw(title1);
    window.draw(title2);
    window.draw(title3);
    window.draw(title4);

    window.display();
}

void Game::renderLevelSelect() {
    window.setView(hudView);
    window.clear();
    
    sf::Sprite levelBack;
    sf::Text level1, level2, level3, back;

    levelBack.setTexture(assets.getTexture("levelBack"));

    level1.setFont(*font);
    level1.setCharacterSize(40);
    level1.setPosition(0, 0);
    level1.setString("Level 1");
    if (!isThisLevelAvailable("./levels/level1.json")) {
        level1.setFillColor(sf::Color::Red);
    }
    else {
        level1.setFillColor(sf::Color::White);
    }

    level2.setFont(*font);
    level2.setCharacterSize(40);
    level2.setPosition(0, 0);
    level2.setString("Level 2");
    if (!isThisLevelAvailable("./levels/level2.json")) {
        level2.setFillColor(sf::Color::Red);
    }
    else {
        level2.setFillColor(sf::Color::White);
    }

    level3.setFont(*font);
    level3.setCharacterSize(40);
    level3.setPosition(0, 0);
    level3.setString("Level 3");
    if (!isThisLevelAvailable("./levels/level3.json")) {
        level3.setFillColor(sf::Color::Red);
    }
    else {
        level3.setFillColor(sf::Color::White);
    }


    back.setFont(*font);
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
        if (isThisLevelAvailable("./levels/level1.json")) {
            level1.setFillColor(sf::Color::Green);
        }
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (isThisLevelAvailable("./levels/level2.json")) {
            level2.setFillColor(sf::Color::Green);
        }
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (isThisLevelAvailable("./levels/level3.json")) {
            level3.setFillColor(sf::Color::Green);
        }
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

void Game::renderRecords() {
    window.setView(hudView);
    window.clear();

    sf::Sprite recordsBack;
    recordsBack.setTexture(assets.getTexture("recordsBack"));

    sf::Text back;
    back.setFont(*font);
    back.setCharacterSize(40);
    back.setFillColor(sf::Color::White);
    back.setPosition(280, 200);
    back.setString("Back");

    std::ifstream inp("./levels/records.json"); 
    if (!inp.is_open()) {
        std::cout << "не удалось открыть файл для записи рекорда\n";
    }
    json parser;
    inp >> parser;
    inp.close();

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);
    if (sf::FloatRect(280.f, 200.f, 70.f, 50.f).contains(mousePosition)) {
        back.setFillColor(sf::Color::Red);
    }

    window.draw(recordsBack);

    sf::Vector2f startPosition(280.f, 50.f);
    for (int i = 0; i < parser.size(); ++i) {
        std::string levelIndex = "level" + std::to_string(i + 1);
        std::string levelData = levelIndex + ": ";
        if (parser[levelIndex].get<float>() == 0.f) {
            levelData += "no record";
        }
        else {
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(1) << parser[levelIndex].get<float>();
            levelData += stream.str();
        }
        sf::Text currentLevelText;
        currentLevelText.setFont(*font);
        currentLevelText.setCharacterSize(40);
        currentLevelText.setFillColor(sf::Color{255, 80, 0});
        currentLevelText.setPosition(startPosition);
        currentLevelText.setString(levelData);
        startPosition.y += 50.f;
        window.draw(currentLevelText);
    }

    window.draw(back);
    window.display();
}

void Game::renderSettings() {
    window.setView(hudView);
    window.clear();
    
    sf::Sprite settingsBack;
    sf::Text title1, title2, back;

    std::string on = "on";
    std::string off = "off";
    std::string text1 = "Sound: " + (isSoundEnabled ? on : off);
    title1.setFont(*font);
    title1.setCharacterSize(40);
    title1.setFillColor(sf::Color::White);
    title1.setPosition(100.f, 30.f);
    title1.setString(text1);
   
    std::string text2 = "Music: " + (isMusicEnabled ? on : off);
    title2.setFont(*font);
    title2.setCharacterSize(40);
    title2.setFillColor(sf::Color::White);
    title2.setPosition(100.f, 80.f);
    title2.setString(text2);

    back.setFont(*font);
    back.setCharacterSize(40);
    back.setFillColor(sf::Color::White);
    back.setPosition(100.f, 130.f);
    back.setString("Back");

    settingsBack.setTexture(assets.getTexture("levelBack"));

    settingsBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        title1.setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        title2.setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        back.setFillColor(sf::Color::Red);
    }

    window.draw(settingsBack);

    window.draw(title1);
    window.draw(title2);
    window.draw(back);

    window.display();
}

void Game::renderPlaying() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
            if (tileMap[i][j] == ' ' || tileMap[i][j] == 'i' || tileMap[i][j] == 'l') {
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
            }
            if (tileMap[i][j] == 'w') {
                s_map.setTextureRect(sf::IntRect(224, 0, 32, 32));
            }
                    
            s_map.setPosition(j * 32, i * 32);

            window.draw(s_map);
        }
    }

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);

    std::string currLevel = getLevelFromPath(currentLevelForRestart);
    currentLevel.setString(currLevel);
    window.setView(window.getDefaultView());
    window.setView(hudView);

    window.draw(currentLevel);
    window.draw(availableTime);
    window.draw(countOfSheep);

    window.display();
}

void Game::renderVictory() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
            if (tileMap[i][j] == ' ' || tileMap[i][j] == 'i' || tileMap[i][j] == 'l') {
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
            }
            if (tileMap[i][j] == 'w') {
                s_map.setTextureRect(sf::IntRect(224, 0, 32, 32));
            }

                    
            s_map.setPosition(j * 32, i * 32);

            window.draw(s_map);
        }
    }

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);
    window.setView(window.getDefaultView());
    
    sf::Sprite victory, buttonToHome, buttonToRestart;
    victory.setTexture(assets.getTexture("buttonsGame"));
    victory.setTextureRect(sf::IntRect(0, 32, 144, 64));
    victory.setPosition(249, 170);
    buttonToHome.setTexture(assets.getTexture("buttonsGame"));
    buttonToHome.setTextureRect(sf::IntRect(32, 0, 32, 32));
    buttonToHome.setPosition(280, 270);
    buttonToRestart.setTexture(assets.getTexture("buttonsGame"));
    buttonToRestart.setTextureRect(sf::IntRect(0, 0, 32, 32));
    buttonToRestart.setPosition(328, 270);

    sf::Text resultTime;
    resultTime.setFont(*font);
    resultTime.setCharacterSize(24);
    resultTime.setFillColor(sf::Color::Black);
    std::string resultTimeText;
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(1) << currentTime;
    resultTimeText += stream.str();
    resultTime.setString(resultTimeText);
    sf::FloatRect bounds = resultTime.getLocalBounds();
    resultTime.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    resultTime.setPosition(320, 240);

    window.setView(hudView);

    std::string currLevel = getLevelFromPath(currentLevelForRestart);
    currentLevel.setString(currLevel);
    window.draw(currentLevel);
    window.draw(resultTime);
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
            
            if (tileMap[i][j] == ' ' || tileMap[i][j] == 'i' || tileMap[i][j] == 'l') {
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
            }
            if (tileMap[i][j] == 'w') {
                s_map.setTextureRect(sf::IntRect(224, 0, 32, 32));
            }

                    
            s_map.setPosition(j * 32, i * 32);

            window.draw(s_map);
        }
    }

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);
    window.setView(window.getDefaultView());

    sf::Sprite defeat, buttonToHome, buttonToRestart;
    defeat.setTexture(assets.getTexture("buttonsGame"));
    defeat.setTextureRect(sf::IntRect(144, 32, 144, 64));
    defeat.setPosition(249, 176);
    buttonToHome.setTexture(assets.getTexture("buttonsGame"));
    buttonToHome.setTextureRect(sf::IntRect(32, 0, 32, 32));
    buttonToHome.setPosition(280, 250);
    buttonToRestart.setTexture(assets.getTexture("buttonsGame"));
    buttonToRestart.setTextureRect(sf::IntRect(0, 0, 32, 32));
    buttonToRestart.setPosition(328, 250);

    window.setView(hudView);

    std::string currLevel = getLevelFromPath(currentLevelForRestart);
    currentLevel.setString(currLevel);
    window.draw(currentLevel);

    window.draw(availableTime);
    window.draw(countOfSheep);
    window.draw(defeat);
    window.draw(buttonToRestart);
    window.draw(buttonToHome);

    window.display();
}

void Game::renderPause() {
    window.setView(view);
    window.clear();

    for (int i = 0; i < mapHeight; ++i) {
        for (int j = 0; j < mapWidth; ++j) {
            
            if (tileMap[i][j] == ' ' || tileMap[i][j] == 'i' || tileMap[i][j] == 'l') {
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
            if (tileMap[i][j] == 't') {
                s_map.setTextureRect(sf::IntRect(192, 0, 32, 32));
            }
            if (tileMap[i][j] == 'w') {
                s_map.setTextureRect(sf::IntRect(224, 0, 32, 32));
            }

                    
            s_map.setPosition(j * 32, i * 32);

            window.draw(s_map);
        }
    }

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);
    window.setView(window.getDefaultView());

    sf::Text pause;
    pause.setFont(*font);
    pause.setCharacterSize(72);
    pause.setFillColor(sf::Color::Red);
    pause.setPosition(210, 206);
    pause.setString("PAUSE");

    window.setView(hudView);

    std::string currLevel = getLevelFromPath(currentLevelForRestart);
    currentLevel.setString(currLevel);
    window.draw(currentLevel);

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
        case GameState::Records : {
            renderRecords();
            break;
        }
        case GameState::Settings : {
            renderSettings();
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
        case GameState::Defeat : {
            renderDefeat();
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
                    else if (state == GameState::Settings) {
                        handleSettingsClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                    else if (state == GameState::Victory) {
                        handleVictoryClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                    else if (state == GameState::Defeat) {
                        handleDefeatClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    }
                    else if (state == GameState::Records) {
                        handleRecordsClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
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
                if (isSoundEnabled) {
                    levelCompleting.play();
                }
                updateRecord(currentTime);
                state = GameState::Victory;
            }
            else if (levelTime - currentTime <= 0.f) {
                std::cout << "the game is defeat...\n";
                state = GameState::Defeat;
                if (isSoundEnabled) {
                    levelFailing.play();
                }
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
                if (isSoundEnabled) {
                    if (player.shouldWalk()) {
                        if (hotiba.getStatus() == sf::Sound::Status::Stopped) {
                            hotiba.play();
                        }
                    }
                    else {
                        hotiba.stop();
                    }
                }
            
                for (int i = 0; i < sheeps.size(); ++i) {
                    sheeps[i].update(time, *this);
                    if (isSoundEnabled) {
                        if (sheeps[i].shouldBleat()) {
                            int j = std::rand() % 3;
                            bleatings[j].play();                        
                        }
                    }
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
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::LevelSelect;
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Records;
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Settings;
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Exit;
    }
}

void Game::handleSettingsClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        isSoundEnabled = isSoundEnabled ? false : true;

        if (!isSoundEnabled) {
            buttonTouching.stop();
            levelCompleting.stop();
            levelFailing.stop();
            hotiba.stop();

            for (auto& i : bleatings) {
                i.stop();
            }
        }
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        isMusicEnabled = isMusicEnabled ? false : true;
        if (isMusicEnabled) {
            assets.getMusic("mainMusic").play();
        }
        else {
            assets.getMusic("mainMusic").pause();
        }
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
}

void Game::handleLevelSelectClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        if (isThisLevelAvailable("./levels/level1.json")) {
            if (loadLevel("./levels/level1.json")) {
                currentTime = 0.f;
                countOfSheepInZagon = 0;
                state = GameState::Playing;
                currentLevelForRestart = "./levels/level1.json";
            }
        }
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        if (isThisLevelAvailable("./levels/level2.json")) {
            if (loadLevel("./levels/level2.json")) {
                currentTime = 0.f;
                countOfSheepInZagon = 0;
                state = GameState::Playing;
                currentLevelForRestart = "./levels/level2.json";
            }
        }
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        
        if (isThisLevelAvailable("./levels/level3.json")) {
            if (loadLevel("./levels/level3.json")) {
                currentTime = 0.f;
                countOfSheepInZagon = 0;
                state = GameState::Playing;
                currentLevelForRestart = "./levels/level3.json";
            }
        }
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
}

void Game::handleVictoryClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(280.f, 270.f, 32.f, 32.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
    if (sf::FloatRect(328.f, 270.f, 32.f, 32.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        if (loadLevel(currentLevelForRestart)) {
            currentTime = 0.f;
            countOfSheepInZagon = 0;
            state = GameState::Playing;
        }
    }
}

void Game::handleDefeatClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(280.f, 250.f, 32.f, 32.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
    if (sf::FloatRect(328.f, 250.f, 32.f, 32.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        if (loadLevel(currentLevelForRestart)) {
            currentTime = 0.f;
            countOfSheepInZagon = 0;
            state = GameState::Playing;
        }
    }
}

void Game::handleRecordsClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (sf::FloatRect(280.f, 200.f, 70.f, 50.f).contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
}

void Game::updateRecord(float timeOfLevel) {
    std::ifstream inp("./levels/records.json"); 
    if (!inp.is_open()) {
        std::cout << "не удалось открыть файл для записи рекорда\n";
        return;
    }

    json parser;
    inp >> parser;
    inp.close();

    std::string numOfLevel = getLevelFromPath(currentLevelForRestart);
    if (numOfLevel.empty()) {
        return;
    }

    if (!parser.contains(numOfLevel)) {
        return;
    }
    float recordTime = parser[numOfLevel].get<float>();
    if (recordTime > timeOfLevel || recordTime == 0.f) {
        parser[numOfLevel] = timeOfLevel;

        std::ofstream out("./levels/records.json");
        if (!out.is_open()) {
            std::cout << "не удалось открыть файл для записи рекорда\n";
            return;
        }

        out << parser;
    }
}

std::string Game::getLevelFromPath(const std::string& str) {
    size_t posStart = str.find("level", str.find("level") + 1);
    if (posStart == std::string::npos) {
        return "";
    }

    size_t posEnd = str.find(".json");
    if (posEnd == std::string::npos) { 
        return "";
    }

    std::string name = str.substr(posStart, posEnd - posStart);

    return name;
}

bool Game::isThisLevelAvailable(const std::string& level) {
    std::string currentLevel = getLevelFromPath(level);
    if (currentLevel == "level1") {
        return true;
    }
    else if (currentLevel == "") {
        return false;
    }

    std::ifstream inp("./levels/records.json"); 
    if (!inp.is_open()) {
        std::cout << "не удалось открыть файл для записи рекорда\n";
        return false;
    }

    json parser;
    inp >> parser;
    inp.close();

    try {
        currentLevel = "level" + std::to_string(std::stoi(currentLevel.replace(0, 5, "")) - 1);
    }
    catch (const std::invalid_argument& er) {
        std::cerr << "ошибка: строка не является числом\n";
        return false;
    }
    catch (const std::out_of_range& er) {
        std::cerr << "ошибка: число вне диапазона\n";
        return false;
    }

    if (parser.contains(currentLevel) && parser[currentLevel].get<float>() > 0.f) {
        return true;
    }

    return false;
}
