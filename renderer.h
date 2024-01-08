#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "map.h"
#include "player.h"

constexpr float SCREEN_W = 1200.0f;
constexpr float SCREEN_H = 675.0f;

class Renderer {
public:
  void init();
  void draw3dView(sf::RenderTarget &target, const Player &player,
                  const Map &map);

private:
  sf::Texture wallTexture;
};

#endif // !_RENDERER_H
