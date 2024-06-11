#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

#include "map.h"
#include "thing.h"

constexpr float PLAYER_TURN_SPEED = 150.f;

class Player {
public:
  Player(Thing *thing);
  void update(float deltaTime, const Map &map);

  Thing *thing;
};

#endif // !_PLAYER_H
