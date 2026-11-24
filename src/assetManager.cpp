#include "assetManager.h"

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <utility>
#include <map>
#include <string>

AssetManager::AssetManager() {
    if (!loadTexture("hero", "./assets/real_hero.png")) {
        std::cerr << "не удалось загрузить тексуру: hero.png\n";
    }

    if (!loadTexture("map", "assets/new_map_3.png")) {
        std::cerr << "не удалось загрузить тексуру: new_map_3.png\n";
    }

    if (!loadTexture("sheep", "./assets/sheep_hi.png")) {
        std::cerr << "не удалось загрузить тексуру: sheep_hi.png\n";
    }

    if (!loadTexture("mainMenu", "./assets/mainMenu.png")) {
        std::cerr << "не удалось загрузить тексуру: mainMenu.png\n";
    }

    if (!loadTexture("levelBack", "./assets/levelBack.png")) {
        std::cerr << "не удалось загрузить тексуру: levelBack.png\n";
    }
    
    if (!loadTexture("buttonsGame", "./assets/buttonsGame.png")) {
        std::cerr << "не удалось загрузить тексуру: buttonsGame.png\n";
    }

    if (!loadTexture("recordsBack", "./assets/recordsBack.png")) {
        std::cerr << "не удалось загрузить тексуру: recordsBack.png\n";
    }

    if (!loadFont("handgeschrieben", "./assets/fonts/handgeschrieben.ttf")) {
        std::cerr << "не удалось загрузить шрифт: handgeschrieben.ttf\n";
    }

    if (!loadMusic("mainMusic", "./assets/music/mainMusic.ogg")) {
        std::cerr << "не удалось загрузить музыку: mainMusic.ogg\n";
    }

    if (!loadSound("touchButton", "./assets/music/touchButton.ogg")) {
        std::cerr << "не удалось загрузить звук: touchButton.ogg\n";
    }

    if (!loadSound("levelCompleting", "./assets/music/victorySound.ogg")) {
        std::cerr << "не удалось загрузить звук: victorySound.ogg\n";
    }

    if (!loadSound("levelFailing", "./assets/music/defeatSound.ogg")) {
        std::cerr << "не удалось загрузить звук: defeatSound.ogg\n";
    }

    if (!loadSound("walking", "./assets/music/hotiba.ogg")) {
        std::cerr << "не удалось загрузить звук: hotiba.ogg\n";
    }

    if (!loadSound("bleating1", "./assets/music/bleating1.ogg")) {
        std::cerr << "не удалось загрузить звук: bleating1.ogg\n";
    }

    if (!loadSound("bleating2", "./assets/music/bleating2.ogg")) {
        std::cerr << "не удалось загрузить звук: bleating2.ogg\n";
    }

    if (!loadSound("bleating3", "./assets/music/bleating3.ogg")) {
        std::cerr << "не удалось загрузить звук: bleating3.ogg\n";
    }
}

bool AssetManager::loadTexture(const std::string& name, const std::string& path) {
    sf::Texture temp;
    if (temp.loadFromFile(path)) {
        textures[name] = std::move(temp);
        return true;
    }

    return false;
}

bool AssetManager::loadFont(const std::string& name, const std::string& path) {
    sf::Font temp;
    if (temp.loadFromFile(path)) {
        fonts[name] = std::move(temp);
        return true;
    }

    return false;
}

bool AssetManager::loadMusic(const std::string& name, const std::string& path) {
    std::unique_ptr<sf::Music> temp = std::make_unique<sf::Music>();
    if (temp->openFromFile(path)) {
        musics[name] = std::move(temp);
        return true;
    }

    return false;
}

bool AssetManager::loadSound(const std::string& name, const std::string& path) {
    sf::SoundBuffer temp;
    if (temp.loadFromFile(path)) {
        sounds[name] = std::move(temp);
        return true;
    }

    return false;
}

const sf::Texture& AssetManager::getTexture(const std::string& name) const {
    return textures.at(name);
}

const sf::Font& AssetManager::getFont(const std::string& name) const {
    return fonts.at(name);
}

sf::Music& AssetManager::getMusic(const std::string& name) const {
    return *musics.at(name);
}

const sf::SoundBuffer& AssetManager::getSound(const std::string& name) const {
    return sounds.at(name);
}
