#ifndef _PLAYER_H
#define _PLAYER_H

#include "map.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

constexpr float PLAYER_TURN_SPEED = 150.f;

class Player {
public:
  void draw(sf::RenderTarget &target, float cellSize);
  void update(float deltaTime, const Map &map);

  sf::Vector2f position;
  float angle;

private:
  bool checkMapCollision(const Map &map, sf::Vector2f newPosition, bool xAxis);
};

#endif // !_PLAYER_H
