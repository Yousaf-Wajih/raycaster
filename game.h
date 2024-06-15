#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <vector>

#include "map.h"
#include "player.h"
#include "renderer.h"
#include "thing.h"

class Game {
public:
  Game(Map &map);
  void update(float dt, Map &map);
  void render(sf::RenderWindow &window, const Map &map, bool view2d);

private:
  std::vector<std::shared_ptr<Thing>> things;
  Player player;
  Renderer renderer;

  float gridSize2d;
};
// hi

#endif // !_GAME_H
