#include "thing.h"
#include "animation.h"
#include "game.h"
#include "map.h"
#include "resources.h"
#include "sound.h"
#include "state.h"

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

void Thing::move(Map &map, sf::Vector2f move) {
  if (move == sf::Vector2f()) return;

  if (size == 0.f) {
    position += move;
    return;
  }

  float xOffset = move.x > 0.f ? size / 2.f : -size / 2.f;
  float yOffset = move.y > 0.f ? size / 2.f : -size / 2.f;
  if (!checkMapCollision(map, {position.x + move.x + xOffset, position.y},
                         true)) {
    position.x += move.x;
  }

  if (!checkMapCollision(map, {position.x, position.y + move.y + yOffset},
                         false)) {
    position.y += move.y;
  }

  setup_blockmap(map);
}

void Thing::setup_blockmap(Map &map) {
  sf::Vector2f halfSize = {size / 2.f, size / 2.f};
  sf::Vector2i start = static_cast<sf::Vector2i>(position - halfSize);
  sf::Vector2i end = static_cast<sf::Vector2i>(position + halfSize);

  std::set<std::tuple<int, int>> coords;
  for (int y = start.y; y <= end.y; y++) {
    for (int x = start.x; x <= end.x; x++) { coords.insert({x, y}); }
  }

  std::set<std::tuple<int, int>> to_remove;
  std::set_difference(blockmapCoords.begin(), blockmapCoords.end(),
                      coords.begin(), coords.end(),
                      std::inserter(to_remove, to_remove.end()));

  std::set<std::tuple<int, int>> to_insert;
  std::set_difference(coords.begin(), coords.end(), blockmapCoords.begin(),
                      blockmapCoords.end(),
                      std::inserter(to_insert, to_insert.end()));

  for (const auto &[x, y] : to_remove) { map.removeFromBlockmap(x, y, this); }
  for (const auto &[x, y] : to_insert) { map.insertInBlockmap(x, y, this); }

  blockmapCoords = coords;
}

bool Thing::checkMapCollision(const Map &map, sf::Vector2f newPosition,
                              bool xAxis) {
  sf::Vector2f size = {this->size / 2.f, this->size / 2.f};
  sf::Vector2f start = newPosition - size;
  sf::Vector2f end = newPosition + size;

  if (xAxis) {
    for (int y = start.y; y <= end.y; y++) {
      if (map.getMapCell(newPosition.x, y, Map::LAYER_WALLS)) { return true; }

      const auto &set = map.getBlockmap(newPosition.x, y);
      for (const auto &thing : set) {
        if (thing->size == 0.f || thing == this) continue;

        sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
        sf::Vector2f thingStart = thing->position - halfSize;
        sf::Vector2f thingEnd = thing->position + halfSize;

        float px = newPosition.x;
        float py0 = start.y, py1 = end.y, magnitude = py1 - py0;
        float t1 = (thingStart.y - py0) / magnitude;
        float t2 = (thingEnd.y - py0) / magnitude;

        float entry = std::fmin(t1, t2);
        float exit = std::fmax(t1, t2);

        bool yIntersect = exit > entry && exit > 0.f && entry < 1.f;
        if (px >= thingStart.x && px <= thingEnd.x && yIntersect) {
          return true;
        }
      }
    }
  } else {
    for (int x = start.x; x <= end.x; x++) {
      if (map.getMapCell(x, newPosition.y, Map::LAYER_WALLS)) { return true; }

      const auto &set = map.getBlockmap(x, newPosition.y);
      for (const auto &thing : set) {
        if (thing->size == 0.f || thing == this) { continue; }

        sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
        sf::Vector2f thingStart = thing->position - halfSize;
        sf::Vector2f thingEnd = thing->position + halfSize;

        float py = newPosition.y;
        float px0 = start.x, px1 = end.x, magnitude = px1 - px0;
        float t1 = (thingStart.x - px0) / magnitude;
        float t2 = (thingEnd.x - px0) / magnitude;

        float entry = std::fmin(t1, t2);
        float exit = std::fmax(t1, t2);

        bool xIntersect = exit > entry && exit > 0.f && entry < 1.f;
        if (py >= thingStart.y && py <= thingEnd.y && xIntersect) {
          return true;
        }
      }
    }
  }

  return false;
}

void Thing::damage(float damage, GameState state) {
  health = std::max(health - damage, 0.f);
  if (thinker) thinker->on_damage(*this, state);
}

enum States {
  STATE_IDLE = -1,
  STATE_RUN = 1,
  STATE_PAIN = 0,
  STATE_DEAD = 2,
};

void monster_update(Thing &thing, GameState state) {
  int anim = thing.animator->getAnim();
  if (anim == STATE_DEAD) return;

  float angle = thing.angle / 180.f * M_PI;
  sf::Vector2f dir{std::cos(angle), std::sin(angle)};

  Thing *player = state.game.getPlayer().thing;
  sf::Vector2f toPlayerDir = player->position - thing.position;

  thing.time -= state.dt;
  if ((anim == STATE_IDLE || anim == STATE_RUN) && thing.time <= 0.f) {
    thing.time = (float)rand() / RAND_MAX * .5f + .5f;
    if (rand() % 12 > 10) {
      sound::play(Resources::sounds["monster_act"], thing.position, 2.f);
    }
  }

  if (anim == STATE_IDLE) {
    float dot = dir.x * toPlayerDir.x + dir.y * toPlayerDir.y;
    if (dot > 0.f) {
      thing.animator->setAnim(STATE_RUN, FinishAction::Loop);
      sound::play(Resources::sounds["monster_alert"], thing.position, 4.f);
    }
  } else if (anim == STATE_RUN) {
    auto path = state.pathfinder.getPath((sf::Vector2i)thing.position,
                                         (sf::Vector2i)player->position);

    auto [x, y] = path[path.size() - 1];

    sf::Vector2f next = {x + .5f, y + .5f};
    sf::Vector2f toPathDir = next - thing.position;
    thing.angle = std::atan2(toPathDir.y, toPathDir.x) / M_PI * 180.f;

    thing.move(state.map, dir * state.dt * 2.f);
  }
}

void monster_on_damage(Thing &thing, GameState state) {
  if (thing.getHealth() <= 0.f) {
    thing.size = 0.f;
    thing.directional = false;
    thing.animator->setAnim(STATE_DEAD);
    sound::play(Resources::sounds["monster_death"], thing.position, 6.f);
  } else if (thing.animator) {
    if (thing.animator->getAnim() == STATE_IDLE) {
      thing.animator->setAnim(STATE_RUN, FinishAction::Loop);
    }

    thing.animator->setAnim(STATE_PAIN, FinishAction::Last);
    sound::play(Resources::sounds["monster_pain"], thing.position, 5.f);
  }
}

std::unordered_map<std::string, std::shared_ptr<Thinker>> thinkers{
    {
        "monster",
        std::make_shared<FunctionThinker>(monster_update, monster_on_damage),
    },
};

std::vector<ThingDef> thingDefs{
    {"player", .4f, ""},
    {"barrel", .5f, "barrel"},
    {"pillar", .4f, "pillar"},
    {"light", 0.f, "light"},

    {
        "monster",
        .5f,
        "monster_idle0",
        true,
        "monster",
        50.f,
        {
            {{.1f, "monster_pain0"}},
            {
                {.0f, "monster_idle0"},
                {.1f, "monster_run1_0"},
                {.2f, "monster_run2_0"},
                {.3f, "monster_run3_0"},
            },
            {
                {.0f, "monster_death0"},
                {.2f, "monster_death1"},
                {.4f, "monster_death2"},
                {.6f, "monster_death3"},
                {.8f, "monster_death4"},
                {1.4f, "monster_death5"},
            },
        },
    },
};
