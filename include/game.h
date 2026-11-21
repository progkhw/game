#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "entity.h"
#include "player.h"
#include "sheep.h"
#include "obstacle.h"
#include "vector.h"

#include <vector>
#include <utility>
#include <string>
#include <map>

const float TILE_SIZE = 32.f;

class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;

public:
    AssetManager() {
        textures["hero"].loadFromFile("./assets/hero.png");
        textures["map"].loadFromFile("./assets/new_map_3.png");
        textures["sheep"].loadFromFile("./assets/sheep_hi.png");
        textures["mainMenu"].loadFromFile("./assets/mainMenu.png");
        textures["levelBack"].loadFromFile("./assets/levelBack.png");
        textures["buttonsGame"].loadFromFile("./assets/buttonsGame.png");
        textures["recordsBack"].loadFromFile("./assets/recordsBack.png");
    }

    const sf::Texture& get(const std::string& name) {
        return textures[name];
    }
};

enum class GameState {
    MainMenu,
    LevelSelect,
    Records,
    Settings,
    Playing,
    Victory,
    Defeat,
    Pause,
    Exit
};

class Game {
private:
    sf::RenderWindow window;
    AssetManager assets;
    GameState state;
    float levelTime = 0.f;
    float currentTime = 0.f;

    Player player;
    std::vector<Sheep> sheeps;
    std::vector<Obstacle> obstacles;
    int countOfSheepInZagon = 0;

    std::string currentLevelForRestart;

    sf::Font font;
    sf::Text availableTime;
    sf::Text countOfSheep;
    sf::Text currentLevel;

    sf::Music mainMusic;
    sf::SoundBuffer buttonTouchingB;
    sf::SoundBuffer levelCompl;
    sf::SoundBuffer levelFail;
    sf::SoundBuffer hotibaB;
    std::vector<sf::SoundBuffer> bleatingsB;
    sf::Sound buttonTouching;
    sf::Sound levelCompleting;
    sf::Sound levelFailing;
    sf::Sound hotiba;
    std::vector<sf::Sound> bleatings;

    bool isMusicEnabled = true;
    bool isSoundEnabled = true;

    sf::Sprite s_map;
    sf::View view;
    sf::View hudView;

    int mapWidth = 0;
    int mapHeight = 0;
    std::vector<std::string> tileMap;

    void renderMainMenu();
    void renderLevelSelect();
    void renderRecords();
    void renderSettings();
    void renderPlaying();
    void renderVictory();
    void renderDefeat();
    void renderPause();

public:
    Game();

    bool loadLevel(const std::string& path);
    bool isSolid(int tx, int ty, bool canPassToZagon = false) const;
    bool isInTargetTile(float x, float y, char typeTile) const;
    bool isGameEnd();

    Player& getPlayer();
    std::vector<Sheep>& getSheeps();
    const std::vector<Obstacle>& getObstacles() const;
    std::pair<int, int> getMapSize() const;

    void run();
    void handleMainMenuClick(sf::Vector2i coord);
    void handleSettingsClick(sf::Vector2i coord);
    void handleLevelSelectClick(sf::Vector2i coord);
    void handleVictoryClick(sf::Vector2i coord);
    void handleDefeatClick(sf::Vector2i coord);
    void handleRecordsClick(sf::Vector2i coord);
    void render();

    void updateRecord(float timeOfLevel);
    bool isThisLevelAvailable(const std::string& level);
    std::string getLevelFromPath(const std::string& str);
};
