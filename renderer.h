#ifndef _RENDERER_H
#define _RENDERER_H

#include <SFML/Graphics/RenderTarget.hpp>

#include "map.h"
#include "player.h"

class Renderer {
public:
  void drawRays(sf::RenderTarget &target, const Player &player, const Map &map);

private:
};

#endif // !_RENDERER_H
