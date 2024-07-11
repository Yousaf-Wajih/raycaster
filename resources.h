#ifndef _RESOURCES_H
#define _RESOURCES_H

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

class Resources {
public:
  static sf::Image texturesImage;
  static sf::Texture textures;
  static sf::Texture sprites;
  static sf::SoundBuffer weaponSound;
};

#endif // !_RESOURCES_H
