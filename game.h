#ifndef _GAME_H
#define _GAME_H

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <memory>
#include <vector>

#include "map.h"
#include "player.h"
#include "renderer.h"
#include "thing.h"

class Game {
public:
  Game(Map &map);
  void update(sf::Window &window, float dt, Map &map, bool game_mode);
  void handleEvent(const sf::Event &event, sf::Window &window);

  void render(sf::RenderWindow &window, const Map &map, bool view2d,
              bool game_mode);

private:
  std::vector<std::shared_ptr<Thing>> things;
  std::unique_ptr<Player> player;
  Renderer renderer;

  bool isMouseCaptured;
  sf::Vector2i lastMousePos;

  float gridSize2d;
};

#endif // !_GAME_H
