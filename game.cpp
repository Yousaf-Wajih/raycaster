#include "game.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <memory>
#include <vector>

#include "map.h"
#include "player.h"
#include "renderer.h"
#include "thing.h"

Game::Game(Map &map)
    : things({
          std::make_shared<Thing>(sf::Vector2f{2.2f, 2.2f}, .4f, -1, 45.f),
          std::make_shared<Thing>(sf::Vector2f{6.9f, 5.8f}, .5f, 0),
          std::make_shared<Thing>(sf::Vector2f{6.9f, 9.8f}, .5f, 1),
          std::make_shared<Thing>(sf::Vector2f{6.9f, 7.8f}, 0.f, 2),
      }),
      player(things[0].get()), renderer(), gridSize2d(64.f) {
  for (const auto &thing : things) {
    sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
    sf::Vector2i start = static_cast<sf::Vector2i>(thing->position - halfSize);
    sf::Vector2i end = static_cast<sf::Vector2i>(thing->position + halfSize);

    for (int y = start.y; y <= end.y; y++) {
      for (int x = start.x; x <= end.x; x++) {
        map.insertInBlockmap(x, y, thing.get());
      }
    }
  }
}

void Game::update(float dt, Map &map) { player.update(dt, map); }

void Game::render(sf::RenderWindow &window, const Map &map, bool view2d) {
  if (view2d) {
    sf::View view = window.getDefaultView();
    view.setCenter(player.thing->position * gridSize2d);
    window.setView(view);
    map.draw(window, gridSize2d, Map::LAYER_WALLS);

    sf::RectangleShape rect, line;
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(2.f);

    for (auto &thing : things) {
      line.setSize({thing->size * gridSize2d * 1.25f, gridSize2d / 30.f});
      line.setPosition(thing->position * gridSize2d);
      line.setRotation(thing->angle);

      rect.setSize(sf::Vector2f(thing->size, thing->size) * gridSize2d);
      rect.setOrigin(rect.getSize() / 2.f);
      rect.setPosition(thing->position * gridSize2d);

      sf::Color color = sf::Color::Green;
      if (player.thing == thing.get()) { color = sf::Color::Yellow; }

      rect.setOutlineColor(color);
      line.setFillColor(color);

      window.draw(rect);
      window.draw(line);
    }
  } else {
    window.setView(window.getDefaultView());
    renderer.draw3dView(window, player.thing->position, player.thing->angle,
                        map, things);
  }
}
