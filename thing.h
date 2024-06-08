#ifndef _THING_H
#define _THING_H

#include <SFML/System/Vector2.hpp>

class Thing {
public:
  Thing(sf::Vector2f position = {}, float size = 0.f, int texture = 0)
      : position(position), size(size), texture(texture) {}

  sf::Vector2f position;
  float size;
  int texture;
};

#endif // !_THING_G
