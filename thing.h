#ifndef _THING_H
#define _THING_H

#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <vector>

class Map;
class Thing;

class Thinker {
public:
  virtual void update(Thing &, Map &, float dt) = 0;
};

class FunctionThinker : public Thinker {
public:
  template <typename Fn> FunctionThinker(const Fn &fn) : fn(fn) {}

  void update(Thing &thing, Map &map, float dt) override { fn(thing, map, dt); }

private:
  std::function<void(Thing &, Map &, float)> fn;
};

class Thing {
public:
  Thing(std::string type = "", sf::Vector2f position = {}, float size = 0.f,
        int texture = 0, float angle = 0.f, bool directional = false)
      : type(type), position(position), size(size), texture(texture),
        angle(angle), directional(directional) {}

  void move(Map &map, sf::Vector2f move);
  void setup_blockmap(Map &map);

  sf::Vector2f position;
  float angle;
  float size;
  int texture;
  bool directional;
  std::string type;

  std::shared_ptr<Thinker> thinker;

private:
  bool checkMapCollision(const Map &map, sf::Vector2f newPosition, bool xAxis);
  std::set<std::tuple<int, int>> blockmap_coords;
};

struct ThingDef {
  std::string name;
  float size;
  int texture;
  bool directional;
};

extern std::vector<ThingDef> thingDefs;

#endif // !_THING_H
