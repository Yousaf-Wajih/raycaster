#include "raycast.h"
#include "map.h"

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>

RayHit raycast(const Map &map, sf::Vector2f pos, sf::Vector2f dir, size_t max) {
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
  for (size_t depth = 0; !hit && depth < max; depth++) {
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
  }

  return RayHit{
      hit,
      mapPos,
      isHitVertical,
      isHitVertical ? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x,
  };
}
