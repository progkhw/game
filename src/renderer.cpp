#include "renderer.h"

Renderer::Renderer(sf::RenderWindow& _window, const AssetManager& _assets, const Player& _player, const std::vector<Sheep>& _sheeps, const std::vector<std::string>& _tileMap, const sf::View& _view, const sf::View& _hudView, const sf::Font& _font, const std::pair<float, float>& _zagonSpawn) 
    : window(_window), assets(_assets), player(_player), sheeps(_sheeps), tileMap(_tileMap), view(_view), hudView(_hudView), font(_font), zagonSpawn(_zagonSpawn) {    
    s_map.setTexture(assets.getTexture("map"));
    zagon.setTexture(assets.getTexture("map"));
    zagon.setTextureRect(sf::IntRect(0, 32, 160, 160));

    createText("availableTime", font, 24, {0, 204, 204}, "", {0, 0});
    createText("countOfSheep", font, 24, {0, 204, 204}, "", {60, 0});
    createText("currentLevel", font, 24, {0, 204, 204}, "", {140, 0});
    createText("mainMenuButton1", font, 40, sf::Color::White, "Playing", {100, 30});
    createText("mainMenuButton2", font, 40, sf::Color::White, "Records", {100, 80});
    createText("mainMenuButton3", font, 40, sf::Color::White, "Settings", {100, 130});
    createText("mainMenuButton4", font, 40, sf::Color::White, "Exit", {100, 180});
    createText("levelSelectButton1", font, 40, sf::Color::White, "level1", {100, 30});
    createText("levelSelectButton2", font, 40, sf::Color::White, "level2", {100, 80});
    createText("levelSelectButton3", font, 40, sf::Color::White, "level3", {100, 130});
    createText("levelSelectButton4", font, 40, sf::Color::White, "back", {100, 180});
    createText("recordsButton1", font, 40, sf::Color::White, "back", {280, 200});
    createText("level1", font, 40, {255, 80, 0}, "", {280, 50});
    createText("level2", font, 40, {255, 80, 0}, "", {280, 100});
    createText("level3", font, 40, {255, 80, 0}, "", {280, 150});
    createText("settingsButton1", font, 40, sf::Color::White, "", {100, 30});
    createText("settingsButton2", font, 40, sf::Color::White, "", {100, 80});
    createText("settingsButton3", font, 40, sf::Color::White, "back", {100, 130});
    createText("victoryResultTime", font, 24, sf::Color::White, "", {});
    createText("playingPause", font, 40, sf::Color::Red, "Pause", {});
}

void Renderer::renderMainMenu() {
    window.setView(hudView);
    window.clear();

    getText("mainMenuButton1").setFillColor(sf::Color::White);
    getText("mainMenuButton2").setFillColor(sf::Color::White);
    getText("mainMenuButton3").setFillColor(sf::Color::White);
    getText("mainMenuButton4").setFillColor(sf::Color::White);
    
    sf::Sprite menuBack;
    menuBack.setTexture(assets.getTexture("mainMenu"));
    menuBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        getText("mainMenuButton1").setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        getText("mainMenuButton2").setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        getText("mainMenuButton3").setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        getText("mainMenuButton4").setFillColor(sf::Color::Red);
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

    getText("levelSelectButton1").setFillColor(sf::Color::White);
    getText("levelSelectButton2").setFillColor(sf::Color::White);
    getText("levelSelectButton3").setFillColor(sf::Color::White);
    getText("levelSelectButton4").setFillColor(sf::Color::White);
    
    sf::Sprite levelBack;
    levelBack.setTexture(assets.getTexture("levelBack"));

    if (!levelsAvailable[0]) {
        getText("levelSelectButton1").setFillColor(sf::Color::Red);
    }
    else {
        getText("levelSelectButton1").setFillColor(sf::Color::White);
    }
    if (!levelsAvailable[1]) {
        getText("levelSelectButton2").setFillColor(sf::Color::Red);
    }
    else {
        getText("levelSelectButton2").setFillColor(sf::Color::White);
    }
    if (!levelsAvailable[2]) {
        getText("levelSelectButton3").setFillColor(sf::Color::Red);
    }
    else {
        getText("levelSelectButton3") .setFillColor(sf::Color::White);
    }

    levelBack.setPosition(0, 0);

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePosition = window.mapPixelToCoords(mousePos, hudView);

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        if (levelsAvailable[0]) {
            getText("levelSelectButton1").setFillColor(sf::Color::Green);
        }
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        if (levelsAvailable[1]) {
            getText("levelSelectButton2").setFillColor(sf::Color::Green);
        }
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        if (levelsAvailable[2]) {
            getText("levelSelectButton3").setFillColor(sf::Color::Green);
        }
    }
    if (sf::FloatRect(100.f, 180.f, 170.f, 50.f).contains(mousePosition)) {
        getText("levelSelectButton4").setFillColor(sf::Color::Red);
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

    getText("recordsButton1").setFillColor(sf::Color::White);

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
    if (sf::FloatRect(280.f, 200.f, 70.f, 50.f).contains(mousePosition)) {
        getText("recordsButton1").setFillColor(sf::Color::Red);
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

    getText("settingsButton1").setFillColor(sf::Color::White);
    getText("settingsButton2").setFillColor(sf::Color::White);
    getText("settingsButton3").setFillColor(sf::Color::White);
    
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

    if (sf::FloatRect(100.f, 30.f, 170.f, 50.f).contains(mousePosition)) {
        getText("settingsButton1").setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 80.f, 170.f, 50.f).contains(mousePosition)) {
        getText("settingsButton2").setFillColor(sf::Color::Blue);
    }
    if (sf::FloatRect(100.f, 130.f, 170.f, 50.f).contains(mousePosition)) {
        getText("settingsButton3").setFillColor(sf::Color::Red);
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

    renderMap();
    window.draw(zagon);
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
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

    getText("victoryResultTime").setString(resultTime);
    sf::FloatRect bounds = getText("victoryResultTime").getLocalBounds();
    getText("victoryResultTime").setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    getText("victoryResultTime").setPosition(hudView.getCenter());

    window.setView(hudView);

    getText("currentLevel").setString(currentLevel);
    window.draw(getText("currentLevel"));
    window.draw(getText("victoryResultTime"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));
    window.draw(victory);
    window.draw(buttonToRestart);
    window.draw(buttonToHome);

    window.display();
}

void Renderer::renderDefeat(const std::string& currentLevel) {
    window.setView(view);
    window.clear();

    renderMap();
    window.draw(zagon);
    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
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

    getText("currentLevel").setString(currentLevel);
    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));
    window.draw(defeat);
    window.draw(buttonToRestart);
    window.draw(buttonToHome);

    window.display();
}

void Renderer::renderPause(const std::string& currentLevel) {
    window.setView(view);
    window.clear();

    renderMap();

    for (int i = 0; i < sheeps.size(); ++i) {
        sheeps[i].draw(window);
    }
    window.draw(zagon);
    player.draw(window);
    window.setView(window.getDefaultView());

    sf::FloatRect textBounds = getText("playingPause").getLocalBounds();
    getText("playingPause").setOrigin(textBounds.width / 2.f, textBounds.height / 2.f);
    getText("playingPause").setPosition(hudView.getCenter());

    window.setView(hudView);

    getText("currentLevel").setString(currentLevel);
    window.draw(getText("currentLevel"));
    window.draw(getText("availableTime"));
    window.draw(getText("countOfSheep"));
    window.draw(getText("playingPause"));

    window.display();
}

void Renderer::createText(const std::string& id, const sf::Font& fontText, int charSize, const sf::Color& color, const std::string& text, const sf::Vector2f& pos) {
    sf::Text temp;
    temp.setFont(fontText);
    temp.setCharacterSize(charSize);
    temp.setFillColor(color);
    temp.setString(text);
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
