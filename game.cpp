#include "game.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <vector>

#include "map.h"
#include "player.h"
#include "renderer.h"
#include "thing.h"

Game::Game(Map &map) : things(), renderer(), gridSize2d(64.f) {
  for (const auto &t : map.getThings()) {
    const auto &def = thingDefs[t.idx];
    std::shared_ptr thing =
        std::make_shared<Thing>(t.position, def.size, def.texture, t.angle);

    things.push_back(thing);
    if (t.idx == 0) { player = std::make_unique<Player>(thing.get()); }
  }

  if (!player) {
    const auto &def = thingDefs[0];
    std::shared_ptr thing =
        std::make_shared<Thing>(sf::Vector2f{}, def.size, def.texture, 0.f);

    things.push_back(thing);
    player = std::make_unique<Player>(thing.get());
  }

  for (const auto &thing : things) { thing->setup_blockmap(map); }
}

void Game::update(float dt, Map &map, bool game_mode) {
  player->update(dt, map, !game_mode);
  if (game_mode) {
    for (auto &thing : things) {
      if (thing->thinker) { thing->thinker->update(*thing, map, dt); }
    }
  }
}

void Game::render(sf::RenderWindow &window, const Map &map, bool view2d,
                  bool game_mode) {
  if (view2d) {
    sf::View view = window.getDefaultView();
    view.setCenter(player->thing->position * gridSize2d);
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
      if (player->thing == thing.get()) {
        color = sf::Color::Yellow;
      } else if (game_mode) {
        continue;
      }

      rect.setOutlineColor(color);
      line.setFillColor(color);

      window.draw(rect);
      window.draw(line);
    }
  } else {
    window.setView(window.getDefaultView());
    renderer.draw3dView(window, player->thing->position, player->thing->angle,
                        map, things, !game_mode);
  }
}
