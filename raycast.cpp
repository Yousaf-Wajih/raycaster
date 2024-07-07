#include "raycast.h"
#include "map.h"
#include "thing.h"

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>

RayHit raycast(const Map &map, sf::Vector2f pos, sf::Vector2f dir, size_t max,
               bool checkThings, Thing *ignore) {
  sf::Vector2f deltaDist{
      std::abs(1.0f / dir.x),
      std::abs(1.0f / dir.y),
  };

  sf::Vector2i mapPos{pos};
  sf::Vector2i step;
  sf::Vector2f sideDist;

  if (dir.x < 0.0f) {
    step.x = -1;
    sideDist.x = (-mapPos.x + pos.x) * deltaDist.x;
  } else {
    step.x = 1;
    sideDist.x = (mapPos.x - pos.x + 1.0f) * deltaDist.x;
  }

  if (dir.y < 0.0f) {
    step.y = -1;
    sideDist.y = (-mapPos.y + pos.y) * deltaDist.y;
  } else {
    step.y = 1;
    sideDist.y = (mapPos.y - pos.y + 1.0f) * deltaDist.y;
  }

  int hit{};
  bool isHitVertical{};
  Thing *hitThing{};

  for (size_t depth = 0; !hitThing && !hit && depth < max; depth++) {
    if (sideDist.x < sideDist.y) {
      sideDist.x += deltaDist.x;
      mapPos.x += step.x;
      isHitVertical = false;
    } else {
      sideDist.y += deltaDist.y;
      mapPos.y += step.y;
      isHitVertical = true;
    }

    hit = map.getMapCell(mapPos.x, mapPos.y, Map::LAYER_WALLS);
    if (hit || !checkThings) continue;

    float origin[2] = {pos.x, pos.y};
    float dir_inv[2] = {1.f / dir.x, 1.f / dir.y};

    auto block = map.getBlockmap(mapPos.x, mapPos.y);
    for (const auto &thing : block) {
      if (thing == ignore) continue;

      sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
      sf::Vector2f start = thing->position - halfSize;
      sf::Vector2f end = thing->position + halfSize;

      float min[2] = {start.x, start.y};
      float max[2] = {end.x, end.y};

      float tmin = 0.f, tmax = std::numeric_limits<float>::infinity();
      for (int d = 0; d < 2; d++) {
        float t1 = (min[d] - origin[d]) * dir_inv[d];
        float t2 = (max[d] - origin[d]) * dir_inv[d];

        tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
        tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));
      }

      if (tmin < tmax) { hitThing = thing; }
    }
  }

  return RayHit{
      hit,
      mapPos,
      isHitVertical,
      isHitVertical ? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x,
      hitThing,
  };
}
