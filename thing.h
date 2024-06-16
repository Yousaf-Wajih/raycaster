#ifndef _THING_H
#define _THING_H

#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>

class Map;
class Thing;

class Thinker {
public:
  virtual void update(Thing &, const Map &, float dt) = 0;
};

class FunctionThinker : public Thinker {
public:
  template <typename Fn> FunctionThinker(const Fn &fn) : fn(fn) {}

  void update(Thing &thing, const Map &map, float dt) override {
    fn(thing, map, dt);
  }

private:
  std::function<void(Thing &, const Map &, float)> fn;
};

class Thing {
public:
  Thing(sf::Vector2f position = {}, float size = 0.f, int texture = 0,
        float angle = 0.f)
      : position(position), size(size), texture(texture), angle(angle) {}

  void update(const Map &map, float dt);
  void move(const Map &map, sf::Vector2f move);

  sf::Vector2f position;
  float angle;
  float size;
  int texture;

  std::shared_ptr<Thinker> thinker;

private:
  bool checkMapCollision(const Map &map, sf::Vector2f newPosition, bool xAxis);
};

#endif // !_THING_H
