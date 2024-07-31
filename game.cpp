#include "game.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Window.hpp>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <vector>

#include "animation.h"
#include "map.h"
#include "player.h"
#include "renderer.h"
#include "resources.h"
#include "sound.h"
#include "state.h"
#include "thing.h"

Game::Game(Map &map)
    : things(), renderer(), gridSize2d(64.f), isMouseCaptured(),
      weaponAnim(&weaponTex, {Animation<sf::Texture *>({
                                 {.0f, &weaponFireTex[0]},
                                 {.15f, &weaponFireTex[1]},
                                 {.3f, &weaponFireTex[2]},
                                 {.45f, &weaponFireTex[3]},
                                 {.6f, &weaponFireTex[2]},
                                 {.75f, &weaponFireTex[1]},
                                 {.9f, &weaponFireTex[0]},
                                 {1.f, &weaponFireTex[0]},
                             })}) {
  for (const auto &t : map.things) {
    const auto &def = thingDefs[t.idx];

    int texture = Resources::getSprite(def.texture);
    std::shared_ptr thing =
        std::make_shared<Thing>(def.name, def.health, t.position, def.size,
                                texture, t.angle, def.directional);
    thing->thinker = thinkers[def.thinker];

    if (!def.animations.empty()) {
      std::vector<Animation<int>> animations;
      for (const auto &anim : def.animations) {
        std::vector<Animation<int>::Keyframe> keyframes;
        for (const auto &[time, tex] : anim) {
          keyframes.push_back({time, Resources::getSprite(tex)});
        }

        animations.push_back({keyframes});
      }

      thing->animator = std::make_unique<Animator<int>>(texture, animations);
    }

    things.push_back(thing);
    if (t.idx == 0) { player = std::make_unique<Player>(thing.get()); }
  }

  if (!player) {
    const auto &def = thingDefs[0];
    std::shared_ptr thing =
        std::make_shared<Thing>("player", 100.f, sf::Vector2f{}, def.size,
                                Resources::getSprite(def.texture), 0.f);

    things.push_back(thing);
    player = std::make_unique<Player>(thing.get());
  }

  weaponTex.loadFromFile("weapon.png");
  weaponFireTex[0].loadFromFile("weapon_fire0.png");
  weaponFireTex[1].loadFromFile("weapon_fire1.png");
  weaponFireTex[2].loadFromFile("weapon_fire2.png");
  weaponFireTex[3].loadFromFile("weapon_fire3.png");

  for (const auto &thing : things) { thing->setup_blockmap(map); }
}

void Game::update(sf::Window &window, float dt, Map &map, bool game_mode) {
  for (const auto &it : to_delete) {
    for (const auto &[x, y] : it->get()->getBlockmapCoords()) {
      map.removeFromBlockmap(x, y, it->get());
    }

    things.erase(it);
  }

  to_delete.clear();

  window.setMouseCursorVisible(!isMouseCaptured);

  std::optional<sf::Vector2i> mouseDelta{};
  if (isMouseCaptured) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    mouseDelta = (mousePos - lastMousePos);
    sf::Mouse::setPosition(lastMousePos, window);
  }

  GameState state{*this, map, dt};
  player->update(dt, state, weaponAnim, mouseDelta, !game_mode);
  if (game_mode) {
    for (auto &thing : things) {
      if (thing->thinker) { thing->thinker->update(*thing, state); }
      if (thing->animator) { thing->animator->update(dt); }
    }
  }

  weaponAnim.update(dt);
  sound::update();
}

void Game::handleEvent(const sf::Event &event, sf::Window &window) {
  switch (event.type) {
  case sf::Event::MouseButtonPressed:
    isMouseCaptured = true;
    lastMousePos = sf::Mouse::getPosition(window);
    break;
  case sf::Event::KeyPressed:
    if (event.key.code == sf::Keyboard::Backspace) { isMouseCaptured = false; }
    break;
  default: break;
  }
}

void Game::render(sf::RenderWindow &window, const Map &map, bool view2d,
                  bool game_mode) {
  if (view2d) {
    sf::Vector2f center = player->thing->position * gridSize2d;
    sf::Vector2f size = static_cast<sf::Vector2f>(window.getSize());
    window.setView(sf::View(center, size));
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
      if (player->thing == thing.get()) { color = sf::Color::Yellow; }
      // else if (game_mode) {
      //   continue;
      // }

      rect.setOutlineColor(color);
      line.setFillColor(color);

      window.draw(rect);
      window.draw(line);
    }
  } else {
    sf::Vector2f size = static_cast<sf::Vector2f>(window.getSize());
    window.setView(sf::View(size / 2.f, size));
    renderer.draw3dView(window, player->thing->position, player->thing->angle,
                        map, things, !game_mode);

    sf::Texture *tex = weaponAnim.get();
    if (tex) {
      sf::Sprite weapon{*tex};
      weapon.setOrigin(tex->getSize().x / 2.f, tex->getSize().y);
      weapon.setPosition(window.getSize().x / 2.f, window.getSize().y);
      weapon.setScale(sf::Vector2f(2.5f, 2.5f) * (window.getSize().x / 960.f));
      window.draw(weapon);
    }
  }
}

void Game::destroy(Thing *thing) {
  for (auto it = things.begin(); it != things.end(); it++) {
    if (it->get() == thing) {
      to_delete.push_back(it);
      break;
    }
  }
}

Player &Game::getPlayer() { return *player; }
