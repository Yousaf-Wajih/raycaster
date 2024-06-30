#include "thing.h"
#include "map.h"

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

void Thing::move(Map &map, sf::Vector2f move) {
  if (move == sf::Vector2f()) return;

  float xOffset = move.x > 0.f ? size / 2.f : -size / 2.f;
  float yOffset = move.y > 0.f ? size / 2.f : -size / 2.f;
  if (!checkMapCollision(
          map, {position.x + move.x + xOffset, position.y}, true)) {
    position.x += move.x;
  }

  if (!checkMapCollision(
          map, {position.x, position.y + move.y + yOffset}, false)) {
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
  std::set_difference(blockmap_coords.begin(),
                      blockmap_coords.end(),
                      coords.begin(),
                      coords.end(),
                      std::inserter(to_remove, to_remove.end()));

  std::set<std::tuple<int, int>> to_insert;
  std::set_difference(coords.begin(),
                      coords.end(),
                      blockmap_coords.begin(),
                      blockmap_coords.end(),
                      std::inserter(to_insert, to_insert.end()));

  for (const auto &[x, y] : to_remove) { map.removeFromBlockmap(x, y, this); }
  for (const auto &[x, y] : to_insert) { map.insertInBlockmap(x, y, this); }

  blockmap_coords = coords;
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

std::vector<ThingDef> thingDefs{
    {"player", .4f, -1, false},
    {"barrel", .5f, 0, false},
    {"pillar", .5f, 1, false},
    {"light", 0.f, 2, false},
    {"monster", .75f, 3, true},
};
