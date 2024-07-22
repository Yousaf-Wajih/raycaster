#include "resources.h"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

sf::Image Resources::texturesImage{};
sf::Texture Resources::textures{};
sf::SoundBuffer Resources::weaponSound{};

sf::Texture Resources::sprites{};
std::unordered_map<std::string, int> Resources::spriteNames{};

int Resources::getSprite(std::string name) {
  auto it = Resources::spriteNames.find(name);
  if (it != Resources::spriteNames.end()) {
    return it->second;
  } else {
    return -1;
  }
}
