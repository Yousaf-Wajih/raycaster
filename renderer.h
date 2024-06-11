#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <cstddef>
#include <vector>

#include "map.h"
#include "player.h"
#include "thing.h"

constexpr float SCREEN_W = 960.0f;
constexpr float SCREEN_H = 540.0f;

class Renderer {
public:
  Renderer();
  void draw3dView(sf::RenderTarget &target, sf::Vector2f position, float angle,
                  const Map &map, std::vector<std::shared_ptr<Thing>> &things);

private:
  sf::Texture skyTexture;

  sf::Texture screenBuffer;
  sf::Sprite screenBufferSprite;
  float zBuffer[(size_t)SCREEN_W];
};

#endif // !_RENDERER_H
