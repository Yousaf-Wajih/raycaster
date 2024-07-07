#ifndef _RAYCAST_H
#define _RAYCAST_H

#include "map.h"
#include "thing.h"

#include <SFML/System/Vector2.hpp>
#include <cstddef>

struct RayHit {
  int cell;
  sf::Vector2i mapPos;
  bool isHitVertical;
  float perpWallDist;

  Thing *thing;
};

RayHit raycast(const Map &map, sf::Vector2f pos, sf::Vector2f dir,
               size_t max = 64, bool checkThings = false,
               Thing *ignore = nullptr);

#endif
