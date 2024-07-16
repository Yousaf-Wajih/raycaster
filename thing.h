#ifndef _THING_H
#define _THING_H

#include <SFML/System/Vector2.hpp>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "state.h"

class Map;
class Thing;

class Thinker {
public:
  virtual void update(Thing &thing, GameState state) = 0;
  virtual void on_damage(Thing &thing, GameState state) = 0;
};

class FunctionThinker : public Thinker {
public:
  FunctionThinker(std::function<void(Thing &, GameState state)> &&update = {},
                  std::function<void(Thing &, GameState state)> &&damage = {})
      : updateFn(update), damageFn(damage) {}

  void update(Thing &thing, GameState state) override {
    if (updateFn) updateFn(thing, state);
  }

  void on_damage(Thing &thing, GameState state) override {
    if (damageFn) damageFn(thing, state);
  }

private:
  std::function<void(Thing &, GameState state)> updateFn;
  std::function<void(Thing &, GameState state)> damageFn;
};

class Thing {
public:
  Thing(std::string type = "", float health = 0.f, sf::Vector2f position = {},
        float size = 0.f, int texture = 0, float angle = 0.f,
        bool directional = false)
      : type(type), health(health), maxHealth(health), position(position),
        size(size), texture(texture), angle(angle), directional(directional) {}

  void move(Map &map, sf::Vector2f move);
  void setup_blockmap(Map &map);

  void damage(float damage, GameState state);

  float getHealth() const { return health; }
  const std::set<std::tuple<int, int>> &getBlockmapCoords() const {
    return blockmapCoords;
  }

  sf::Vector2f position;
  float angle;
  float size;
  int texture;
  bool directional;
  std::string type;

  std::shared_ptr<Thinker> thinker;

private:
  bool checkMapCollision(const Map &map, sf::Vector2f newPosition, bool xAxis);
  std::set<std::tuple<int, int>> blockmapCoords;

  float health, maxHealth;
};

struct ThingDef {
  std::string name;
  float size;
  int texture;
  bool directional;
  std::string thinker;
  float health;
};

extern std::unordered_map<std::string, std::shared_ptr<Thinker>> thinkers;
extern std::vector<ThingDef> thingDefs;

#endif // !_THING_H
