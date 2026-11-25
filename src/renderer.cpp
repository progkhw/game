#include "renderer.h"

Renderer::Renderer(sf::RenderWindow& _window, const AssetManager& _assets, const Player& _player, const std::vector<Sheep>& _sheeps, const std::vector<std::string>& _tileMap, const sf::View& _view, const sf::View& _hudView, const sf::Font& _font, const std::pair<float, float>& _zagonSpawn) 
    : window(_window), assets(_assets), player(_player), sheeps(_sheeps), tileMap(_tileMap), view(_view), hudView(_hudView), font(_font), zagonSpawn(_zagonSpawn) {    
    s_map.setTexture(assets.getTexture("map"));
    zagon.setTexture(assets.getTexture("map"));
    zagon.setTextureRect(sf::IntRect(0, 32, 160, 160));

    sf::Vector2f huvic = hudView.getCenter();

    createText("availableTime", font, 24, {2, 151, 151}, "", false, {0, 0});
    createText("countOfSheep", font, 24, {2, 151, 151}, "", false, {60, 0});
    createText("currentLevel", font, 24, {2, 151, 151}, "", false, {140, 0});
    createText("mainMenuButton1", font, 40, {0, 204, 204}, "Playing", false, {100, 30});
    createText("mainMenuButton2", font, 40, {0, 204, 204}, "Records", false, {100, 80});
    createText("mainMenuButton3", font, 40, {0, 204, 204}, "Settings", false, {100, 130});
    createText("mainMenuButton4", font, 40, {0, 204, 204}, "Exit", false, {100, 180});
    createText("levelSelectButton1", font, 40, {0, 204, 204}, "level1", false, {100, 30});
    createText("levelSelectButton2", font, 40, {0, 204, 204}, "level2", false, {100, 80});
    createText("levelSelectButton3", font, 40, {0, 204, 204}, "level3", false, {100, 130});
    createText("levelSelectButton4", font, 40, {0, 204, 204}, "Back", false, {100, 180});
    createText("recordsButton1", font, 40, {0, 204, 204}, "Back", false, {280, 200});
    createText("level1", font, 40, {220, 90, 20}, "", false, {280, 50});
    createText("level2", font, 40, {220, 90, 20}, "", false, {280, 100});
    createText("level3", font, 40, {220, 90, 20}, "", false, {280, 150});
    createText("settingsButton1", font, 40, {0, 204, 204}, "", false, {100, 30});
    createText("settingsButton2", font, 40, {0, 204, 204}, "", false, {100, 80});
    createText("settingsButton3", font, 40, {0, 204, 204}, "Back", false, {100, 130});

    createText("playingPause", font, 24, {0, 204, 204}, "Pause", true, {huvic.x, huvic.y - 20});
    createText("playingPauseButton1", font, 24, {0, 204, 204}, "Menu", true, {huvic.x - 50, huvic.y + 20});
    createText("playingPauseButton2", font, 24, {0, 204, 204}, "Restart", true, {huvic.x + 50, huvic.y + 20});

    createText("playingVictory", font, 24, {0, 204, 204}, "Victory", true, {huvic.x, huvic.y - 30});
    createText("playingVictoryTime", font, 24, {0, 204, 204}, "", true, huvic);
    createText("playingVictoryButton1", font, 24, {0, 204, 204}, "Menu", true, {huvic.x - 45, huvic.y + 30});
    createText("playingVictoryButton2", font, 24, {0, 204, 204}, "Restart", true, {huvic.x + 45, huvic.y + 30});

    createText("playingDefeat", font, 24, {0, 204, 204}, "Defeat", true, {huvic.x, huvic.y - 20});
    createText("playingDefeatButton1", font, 24, {0, 204, 204}, "Menu", true, {huvic.x - 50, huvic.y + 20});
    createText("playingDefeatButton2", font, 24, {0, 204, 204}, "Restart", true, {huvic.x + 50, huvic.y + 20});
}

void Renderer::renderMainMenu() {
    window.setView(hudView);
    window.clear();

    getText("mainMenuButton1").setFillColor({0, 204, 204});
    getText("mainMenuButton2").setFillColor({0, 204, 204});
    getText("mainMenuButton3").setFillColor({0, 204, 204});
    getText("mainMenuButton4").setFillColor({0, 204, 204});
    
    sf::Sprite menuBack;
    menuBack.setTexture(assets.getTexture("mainMenu"));
    menuBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (getText("mainMenuButton1").getGlobalBounds().contains(mousePosition)) {
        getText("mainMenuButton1").setFillColor({2, 151, 151});
    }
    if (getText("mainMenuButton2").getGlobalBounds().contains(mousePosition)) {
        getText("mainMenuButton2").setFillColor({2, 151, 151});
    }
    if (getText("mainMenuButton3").getGlobalBounds().contains(mousePosition)) {
        getText("mainMenuButton3").setFillColor({2, 151, 151});
    }
    if (getText("mainMenuButton4").getGlobalBounds().contains(mousePosition)) {
        getText("mainMenuButton4").setFillColor({2, 151, 151});
    }

    window.draw(menuBack);

    window.draw(getText("mainMenuButton1"));
    window.draw(getText("mainMenuButton2"));
    window.draw(getText("mainMenuButton3"));
    window.draw(getText("mainMenuButton4"));

    window.display();
}

void Renderer::renderLevelSelect(const std::vector<bool>& levelsAvailable) {
    window.setView(hudView);
    window.clear();

    getText("levelSelectButton1").setFillColor({0, 204, 204});
    getText("levelSelectButton2").setFillColor({0, 204, 204});
    getText("levelSelectButton3").setFillColor({0, 204, 204});
    getText("levelSelectButton4").setFillColor({0, 204, 204});
    
    sf::Sprite levelBack;
    levelBack.setTexture(assets.getTexture("levelBack"));

    if (!levelsAvailable[0]) {
        getText("levelSelectButton1").setFillColor({3, 125, 125});
    }
    else {
        getText("levelSelectButton1").setFillColor({0, 204, 204});
    }
    if (!levelsAvailable[1]) {
        getText("levelSelectButton2").setFillColor({3, 125, 125});
    }
    else {
        getText("levelSelectButton2").setFillColor({0, 204, 204});
    }
    if (!levelsAvailable[2]) {
        getText("levelSelectButton3").setFillColor({3, 125, 125});
    }
    else {
        getText("levelSelectButton3") .setFillColor({0, 204, 204});
    }

    levelBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (getText("levelSelectButton1").getGlobalBounds().contains(mousePosition)) {
        if (levelsAvailable[0]) {
            getText("levelSelectButton1").setFillColor({2, 151, 151});
        }
    }
    if (getText("levelSelectButton2").getGlobalBounds().contains(mousePosition)) {
        if (levelsAvailable[1]) {
            getText("levelSelectButton2").setFillColor({2, 151, 151});
        }
    }
    if (getText("levelSelectButton3").getGlobalBounds().contains(mousePosition)) {
        if (levelsAvailable[2]) {
            getText("levelSelectButton3").setFillColor({2, 151, 151});
        }
    }
    if (getText("levelSelectButton4").getGlobalBounds().contains(mousePosition)) {
        getText("levelSelectButton4").setFillColor({2, 151, 151});
    }

    window.draw(levelBack);

    window.draw(getText("levelSelectButton1"));
    window.draw(getText("levelSelectButton2"));
    window.draw(getText("levelSelectButton3"));
    window.draw(getText("levelSelectButton4"));

    window.display();
}

void Renderer::renderRecords() {
    window.setView(hudView);
    window.clear();

    getText("recordsButton1").setFillColor({0, 204, 204});

    sf::Sprite recordsBack;
    recordsBack.setTexture(assets.getTexture("recordsBack"));

    std::ifstream inp("./levels/records.json"); 
    if (!inp.is_open()) {
        std::cout << "не удалось открыть файл для записи рекорда\n";
        return;
    }
    json parser;
    inp >> parser;
    inp.close();

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);
    if (getText("recordsButton1").getGlobalBounds().contains(mousePosition)) {
        getText("recordsButton1").setFillColor({2, 151, 151});
    }

    window.draw(recordsBack);
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
        getText(levelIndex).setString(levelData);
        window.draw(getText(levelIndex));
    }

    window.draw(getText("recordsButton1"));
    window.display();
}

void Renderer::renderSettings(bool isSoundEnabled, bool isMusicEnabled) {
    window.setView(hudView);
    window.clear();

    getText("settingsButton1").setFillColor({0, 204, 204});
    getText("settingsButton2").setFillColor({0, 204, 204});
    getText("settingsButton3").setFillColor({0, 204, 204});
    
    sf::Sprite settingsBack;
    settingsBack.setTexture(assets.getTexture("levelBack"));
    settingsBack.setPosition(0, 0);

    std::string on = "on";
    std::string off = "off";
    std::string text1 = "Sound: " + (isSoundEnabled ? on : off);
    getText("settingsButton1").setString(text1);
   
    std::string text2 = "Music: " + (isMusicEnabled ? on : off);
    getText("settingsButton2").setString(text2);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (getText("settingsButton1").getGlobalBounds().contains(mousePosition)) {
        getText("settingsButton1").setFillColor({2, 151, 151});
    }
    if (getText("settingsButton2").getGlobalBounds().contains(mousePosition)) {
        getText("settingsButton2").setFillColor({2, 151, 151});
    }
    if (getText("settingsButton3").getGlobalBounds().contains(mousePosition)) {
        getText("settingsButton3").setFillColor({2, 151, 151});
    }

    window.draw(settingsBack);
    window.draw(getText("settingsButton1"));
    window.draw(getText("settingsButton2"));
    window.draw(getText("settingsButton3"));
    window.display();
}

void Renderer::renderMap() {
    for (int i = 0; i < tileMap.size(); ++i) {
        for (int j = 0; j < tileMap[0].size(); ++j) {
            
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
}

void Renderer::renderPlaying(const std::string& currentLevel) {
    window.setView(view);
    window.clear();

    renderMap();
    window.draw(zagon);
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    player.draw(window);

    getText("currentLevel").setString(currentLevel);
    window.setView(window.getDefaultView());
    window.setView(hudView);

    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));

    window.display();
}

void Renderer::renderVictory(const std::string& resultTime, const std::string& currentLevel) {
    window.setView(view);
    window.clear();

    getText("playingVictoryButton1").setFillColor({0, 204, 204});
    getText("playingVictoryButton2").setFillColor({0, 204, 204});

    renderMap();
    window.draw(zagon);
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    player.draw(window);
    window.setView(window.getDefaultView());

    window.setView(hudView);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);
    if (getText("playingVictoryButton1").getGlobalBounds().contains(mousePosition)) {
        getText("playingVictoryButton1").setFillColor({2, 151, 151});
    }
    if (getText("playingVictoryButton2").getGlobalBounds().contains(mousePosition)) {
        getText("playingVictoryButton2").setFillColor({2, 151, 151});
    }

    getText("playingVictoryTime").setString(resultTime);
    getText("playingVictoryTime").setOrigin(getText("playingVictoryTime").getLocalBounds().width / 2.f, getText("playingVictoryTime").getLocalBounds().height / 2.f);
    getText("currentLevel").setString(currentLevel);
    window.draw(getText("playingVictory"));
    window.draw(getText("playingVictoryTime"));
    window.draw(getText("playingVictoryButton1"));
    window.draw(getText("playingVictoryButton2"));
    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));

    window.display();
}

void Renderer::renderDefeat(const std::string& currentLevel) {
    window.setView(view);
    window.clear();

    getText("playingDefeatButton1").setFillColor({0, 204, 204});
    getText("playingDefeatButton2").setFillColor({0, 204, 204});
    
    renderMap();
    window.draw(zagon);
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    player.draw(window);
    window.setView(window.getDefaultView());

    window.setView(hudView);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);
    if (getText("playingDefeatButton1").getGlobalBounds().contains(mousePosition)) {
        getText("playingDefeatButton1").setFillColor({2, 151, 151});
    }
    if (getText("playingDefeatButton2").getGlobalBounds().contains(mousePosition)) {
        getText("playingDefeatButton2").setFillColor({2, 151, 151});
    }

    getText("currentLevel").setString(currentLevel);
    window.draw(getText("playingDefeat"));
    window.draw(getText("playingDefeatButton1"));
    window.draw(getText("playingDefeatButton2"));
    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));

    window.display();
}

void Renderer::renderPause(const std::string& currentLevel) {
    window.setView(view);
    window.clear();
    
    getText("playingPauseButton1").setFillColor({0, 204, 204});
    getText("playingPauseButton2").setFillColor({0, 204, 204});

    renderMap();
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);
    window.setView(window.getDefaultView());

    window.setView(hudView);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);
    if (getText("playingPauseButton1").getGlobalBounds().contains(mousePosition)) {
        getText("playingPauseButton1").setFillColor({2, 151, 151});
    }
    if (getText("playingPauseButton2").getGlobalBounds().contains(mousePosition)) {
        getText("playingPauseButton2").setFillColor({2, 151, 151});
    }

    getText("currentLevel").setString(currentLevel);
    window.draw(getText("playingPause"));
    window.draw(getText("playingPauseButton1"));
    window.draw(getText("playingPauseButton2"));
    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));

    window.display();
}

void Renderer::createText(const std::string& id, const sf::Font& fontText, int charSize, const sf::Color& color, const std::string& text, bool isSetOrigin, const sf::Vector2f& pos) {
    sf::Text temp;
    temp.setFont(fontText);
    temp.setCharacterSize(charSize);
    temp.setFillColor(color);
    temp.setString(text);

    if (isSetOrigin) {
        temp.setOrigin(temp.getLocalBounds().width / 2.f, temp.getLocalBounds().height / 2.f);
    }

    temp.setPosition(pos);

    texts[id] = std::move(temp);
}

sf::Text& Renderer::getText(const std::string& id) {
    return texts.at(id);
}

void Renderer::updatePositionZagon(const std::pair<float, float>& newZagonSpawn) {
    zagonSpawn = newZagonSpawn;
    zagon.setPosition(zagonSpawn.first * 32.f, zagonSpawn.second * 32.f);
}

void Renderer::updateHudData(const std::string& availableTime, const std::string& countOfSheep) {
    getText("availableTime").setString(availableTime);
    getText("countOfSheep").setString(countOfSheep);
}
