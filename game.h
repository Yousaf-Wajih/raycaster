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
  void update(float dt, Map &map, bool game_mode);
  void render(sf::RenderWindow &window, const Map &map, bool view2d,
              bool game_mode);

private:
  std::vector<std::shared_ptr<Thing>> things;
  Player player;
  Renderer renderer;

  float gridSize2d;
};

#endif // !_GAME_H
