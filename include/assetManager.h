#pragma once 

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

class AssetManager {
private:
    std::map<std::string, sf::Texture> textures;
    std::map<std::string, sf::Font> fonts;
    std::map<std::string, std::unique_ptr<sf::Music>> musics;
    std::map<std::string, sf::SoundBuffer> sounds;

public:
    AssetManager();
    bool loadTexture(const std::string& name, const std::string& path);
    bool loadFont(const std::string& name, const std::string& path);
    bool loadMusic(const std::string& name, const std::string& path);
    bool loadSound(const std::string& name, const std::string& path);
    const sf::Texture& getTexture(const std::string& name) const;
    const sf::Font& getFont(const std::string& name) const;
    sf::Music& getMusic(const std::string& name) const;
    const sf::SoundBuffer& getSound(const std::string& name) const;
};

