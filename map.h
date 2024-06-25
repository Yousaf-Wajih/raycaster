#ifndef _MAP_H
#define _MAP_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "thing.h"

struct MapThing {
  size_t idx;

  sf::Vector2f position;
  float angle;
};

class Map {
public:
  static constexpr int LAYER_WALLS = 0;
  static constexpr int LAYER_FLOOR = 1;
  static constexpr int LAYER_CEILING = 2;
  static constexpr int NUM_LAYERS = 3;

  static constexpr const char *LAYER_NAMES[NUM_LAYERS] = {
      "Walls",
      "Floor",
      "Ceiling",
  };

  Map() = default;

  int getMapCell(int x, int y, int layer) const;
  void setMapCell(int x, int y, int layer, int value);

  size_t getWidth() const;
  size_t getHeight() const;
  void draw(sf::RenderTarget &target, float cellSize, int layer,
            uint8_t alpha = 255) const;

  void fill(int layer, int value);
  void resize(size_t width, size_t height);

  void insertInBlockmap(int x, int y, Thing *thing);
  void removeFromBlockmap(int x, int y, Thing *thing);
  std::set<Thing *> getBlockmap(int x, int y) const;

  void load(const std::filesystem::path &path);
  void save(const std::filesystem::path &path) const;

  std::vector<MapThing> things;

private:
  std::vector<std::vector<std::array<int, NUM_LAYERS>>> grid;
  std::vector<std::vector<std::set<Thing *>>> blockmap;
};

#endif // !_MAP_H
