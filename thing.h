#ifndef _THING_H
#define _THING_H

#include <SFML/System/Vector2.hpp>

class Map;

class Thing {
public:
  Thing(sf::Vector2f position = {}, float size = 0.f, int texture = 0,
        float angle = 0.f)
      : position(position), size(size), texture(texture), angle(angle) {}

  void move(const Map &map, sf::Vector2f move);

  sf::Vector2f position;
  float angle;
  float size;
  int texture;

private:
  bool checkMapCollision(const Map &map, sf::Vector2f newPosition, bool xAxis);
};

#endif // !_THING_G
