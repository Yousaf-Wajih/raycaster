#ifndef _MAP_H
#define _MAP_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <array>
#include <cstddef>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

class Map {
public:
  static constexpr int LAYER_WALLS = 0;
  static constexpr int LAYER_FLOOR = 1;
  static constexpr int LAYER_CEILING = 2;
  static constexpr int NUM_LAYERS = 3;

  Map(float cellSize);

  float getCellSize() const;
  int getMapCell(int x, int y, int layer) const;
  void setMapCell(int x, int y, int layer, int value);

  void draw(sf::RenderTarget &target, int layer) const;
  void load(const std::filesystem::path &path);
  void save(const std::filesystem::path &path) const;

private:
  std::vector<std::vector<std::array<int, NUM_LAYERS>>> grid;
  float cellSize;
};

#endif // !_MAP_H
