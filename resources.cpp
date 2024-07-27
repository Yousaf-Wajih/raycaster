#include "resources.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

sf::Image Resources::texturesImage{};
sf::Texture Resources::textures{};

sf::Texture Resources::sprites{};
std::unordered_map<std::string, int> Resources::spriteNames{};

std::unordered_map<std::string, sf::SoundBuffer> Resources::sounds;

int Resources::getSprite(std::string name) {
  auto it = Resources::spriteNames.find(name);
  if (it != Resources::spriteNames.end()) {
    return it->second;
  } else {
    return -1;
  }
}
