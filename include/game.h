#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "entity.h"
#include "assetManager.h"
#include "player.h"
#include "sheep.h"
#include "obstacle.h"
#include "vector.h"

#include <vector>
#include <map>
#include <utility>
#include <string>

const float TILE_SIZE = 32.f;

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
    std::pair<float, float> zagonSpawn;
    int countOfSheepInZagon = 0;

    std::string currentLevelForRestart;

    const sf::Font* font = nullptr;
    std::map<std::string, sf::Text> texts;

    sf::Sound buttonTouching;
    sf::Sound levelCompleting;
    sf::Sound levelFailing;
    sf::Sound hotiba;
    std::vector<sf::Sound> bleatings;

    bool isMusicEnabled = true;
    bool isSoundEnabled = true;

    sf::Sprite s_map;
    sf::Sprite zagon;
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

    void createText(const std::string& id, const sf::Font& fontText, int charSize, const sf::Color& color, const std::string& text, const sf::Vector2f& pos);
    sf::Text& getText(const std::string& id);

    void updateRecord(float timeOfLevel);
    bool isThisLevelAvailable(const std::string& level);
    std::string getLevelFromPath(const std::string& str);
};
