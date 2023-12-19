#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>

#include "map.h"
#include "player.h"

constexpr float SCREEN_W = 1200.0f;
constexpr float SCREEN_H = 675.0f;

class Renderer {
public:
  void draw3dView(sf::RenderTarget &target, const Player &player,
                  const Map &map);
  void drawRays(sf::RenderTarget &target, const Player &player, const Map &map);

private:
};

#endif // !_RENDERER_H
