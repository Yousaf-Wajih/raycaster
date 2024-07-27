#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>

class Resources {
public:
  static sf::Image texturesImage;
  static sf::Texture textures;

  static sf::Texture sprites;
  static std::unordered_map<std::string, int> spriteNames;

  static std::unordered_map<std::string, sf::SoundBuffer> sounds;

  static int getSprite(std::string name);
};

#endif // !_RESOURCES_H
