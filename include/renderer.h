#pragma once

#include "assetManager.h"
#include "player.h"
#include "sheep.h"

#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

class Renderer {
private:
    sf::RenderWindow& window;
    const AssetManager& assets;
    const Player& player;
    const std::vector<Sheep>& sheeps;
    const std::vector<std::string>& tileMap;
    const sf::View& view;
    const sf::View& hudView;
    const sf::Font& font;

    std::pair<float, float> zagonSpawn;
    sf::Sprite s_map;
    sf::Sprite zagon;
    std::map<std::string, sf::Text> texts;

public:
    Renderer(sf::RenderWindow& _window, const AssetManager& _assets, const Player& _player, const std::vector<Sheep>& _sheeps, const std::vector<std::string>& _tileMap, const sf::View& _view, const sf::View& _hudView, const sf::Font& _font, const std::pair<float, float>& _zagonSpawn);
    void renderMainMenu();
    void renderLevelSelect(const std::vector<bool>& levelsAvailable);
    void renderRecords();
    void renderSettings(bool isSoundEnabled, bool isMusicEnabled);
    void renderMap();
    void renderPlaying(const std::string& currentLevel);
    void renderVictory(const std::string& resultTime, const std::string& currentLevel);
    void renderDefeat(const std::string& currentLevel);
    void renderPause(const std::string& currentLevel);

    void createText(const std::string& id, const sf::Font& fontText, int charSize, const sf::Color& color, const std::string& text, bool isSetOrigin, const sf::Vector2f& pos);
    sf::Text& getText(const std::string& id);

    void updatePositionZagon(const std::pair<float, float>& newZagonSpawn);
    void updateHudData(const std::string& availableTime, const std::string& countOfSheep);
};
