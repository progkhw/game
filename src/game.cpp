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
    player(640, 400, 64, 96, assets.getTexture("hero")),
    view(sf::FloatRect(0, 0, 640, 480)),
    hudView(sf::FloatRect(0, 0, 640, 480)),
    renderer(window, assets, player, sheeps, tileMap, view, hudView, assets.getFont("handgeschrieben"), {0, 0}) {

    window.setFramerateLimit(60);
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

    bool loading = loadLevel("./levels/level1.json");
    if (!loading) {
        std::cerr << "карта не загружена\n";
    }
}

bool Game::loadLevel(const std::string& path) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        std::cout << "file not opened!\n";
        return false;
    }

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

    for (const auto& i : mapSchema) {
        if (i.size() != mapSchema[0].size()) {
            return false;
        }
    }

    obstacles.clear();
    player.setPosition(Vector(playerSpawnCoor.first * TILE_SIZE, playerSpawnCoor.second * TILE_SIZE));
    sheeps.clear();
    for (const auto& i : sheepSpawnCoor) {
        Sheep sheep(i.first * TILE_SIZE, i.second * TILE_SIZE, 32, 32, assets.getTexture("sheep"));
        sheeps.push_back(sheep);
    }

    tileMap = mapSchema;
    renderer.updatePositionZagon(zagonSpawn);

    for (int i = 0; i < tileMap.size(); ++i) {
        for (int j = 0; j < tileMap[0].size(); ++j) {
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
    if (tx < 0 || tx >= tileMap[0].size() || ty < 0 || ty >= tileMap.size()) {
        return true;
    }

    char c = tileMap[ty][tx];
    return c == '0' || c == 'l' || (!canPassToZagon && c == 'i');
}

bool Game::isInTargetTile(float x, float y, char typeTile) const {
    int posTileX = std::floor(x / TILE_SIZE);
    int posTileY = std::floor(y / TILE_SIZE);

    if (posTileX < 0 || posTileX >= tileMap[0].size() || posTileY < 0 || posTileY >= tileMap.size()) {
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
    return {tileMap[0].size(), tileMap.size()};
}

void Game::render() {
    switch (state) {
        case GameState::MainMenu : {
            renderer.renderMainMenu();
            break;
        }
        case GameState::LevelSelect : {
            std::vector<bool> levelsAvailable;
            for (int i = 0; i < 3; ++i) {
                std::string levelName = "./levels/level" + std::to_string(i + 1) + ".json";
                levelsAvailable.push_back(isThisLevelAvailable(levelName));
            }
            renderer.renderLevelSelect(levelsAvailable);
            break;
        }
        case GameState::Records : {
            renderer.renderRecords();
            break;
        }
        case GameState::Settings : {
            renderer.renderSettings(isSoundEnabled, isMusicEnabled);
            break;
        }
        case GameState::Playing : {
            std::string currLevel = getLevelFromPath(currentLevelForRestart);
            renderer.renderPlaying(currLevel);
            break;
        }
        case GameState::Victory : {
            std::string resultTimeText;
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(1) << currentTime;
            resultTimeText += stream.str();

            std::string currLevel = getLevelFromPath(currentLevelForRestart);
            renderer.renderVictory(resultTimeText, currLevel);
            break;
        }
        case GameState::Defeat : {
            std::string currLevel = getLevelFromPath(currentLevelForRestart);
            renderer.renderDefeat(currLevel);
            break;
        }
        case GameState::Pause : {
            std::string currLevel = getLevelFromPath(currentLevelForRestart);
            renderer.renderPause(currLevel);
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
                    else if (state == GameState::Pause) {
                        handlePauseClick(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
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
    
                if (tempX > (tileMap[0].size() * TILE_SIZE) - 320) {
                    tempX = (tileMap[0].size() * TILE_SIZE) - 320;
                }

                if (tempY < 240) {
                    tempY = 240;
                }

                if (tempY > (tileMap.size() * TILE_SIZE) - 240) {
                    tempY = (tileMap.size() * TILE_SIZE) - 240;
                }

                view.setCenter(tempX, tempY);

                window.setView(view);
            }
        }
        if (state == GameState::Exit) {
            window.close();
            break;
        }
        if (state != GameState::Playing || hotiba.getStatus() == sf::Sound::Status::Stopped) {
            hotiba.stop();
        }

        std::string availTime = std::to_string(static_cast<int>(std::ceil(levelTime - currentTime)));
        std::string cosiz = std::to_string(sheeps.size() - countOfSheepInZagon);
        cosiz = cosiz + " / " + std::to_string(sheeps.size());
        renderer.updateHudData(availTime, cosiz);

        render();
    }
}

void Game::handleMainMenuClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (renderer.getText("mainMenuButton1").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::LevelSelect;
    }
    if (renderer.getText("mainMenuButton2").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Records;
    }
    if (renderer.getText("mainMenuButton3").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Settings;
    }
    if (renderer.getText("mainMenuButton4").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }
        state = GameState::Exit;
    }
}

void Game::handleSettingsClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (renderer.getText("settingsButton1").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("settingsButton2").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("settingsButton3").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
}

void Game::handleLevelSelectClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (renderer.getText("levelSelectButton1").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("levelSelectButton2").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("levelSelectButton3").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("levelSelectButton4").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
}

void Game::handleVictoryClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (renderer.getText("playingVictoryButton1").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
    if (renderer.getText("playingVictoryButton2").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("playingDefeatButton1").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
    if (renderer.getText("playingDefeatButton2").getGlobalBounds().contains(mousePosition)) {
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

void Game::handlePauseClick(sf::Vector2i coord) {
    sf::Vector2f mousePosition = window.mapPixelToCoords(coord, hudView);
    if (renderer.getText("playingPauseButton1").getGlobalBounds().contains(mousePosition)) {
        if (isSoundEnabled) {
            buttonTouching.play();
        }

        state = GameState::MainMenu;
    }
    if (renderer.getText("playingPauseButton2").getGlobalBounds().contains(mousePosition)) {
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
    if (renderer.getText("recordsButton1").getGlobalBounds().contains(mousePosition)) {
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
    std::string currentLevelTemp = getLevelFromPath(level);
    if (currentLevelTemp == "level1") {
        return true;
    }
    else if (currentLevelTemp == "") {
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
        currentLevelTemp = "level" + std::to_string(std::stoi(currentLevelTemp.replace(0, 5, "")) - 1);
    }
    catch (const std::invalid_argument& er) {
        std::cerr << "ошибка: строка не является числом\n";
        return false;
    }
    catch (const std::out_of_range& er) {
        std::cerr << "ошибка: число вне диапазона\n";
        return false;
    }

    if (parser.contains(currentLevelTemp) && parser[currentLevelTemp].get<float>() > 0.f) {
        return true;
    }

    return false;
}
