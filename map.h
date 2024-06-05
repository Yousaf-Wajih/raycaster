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

  static constexpr const char *LAYER_NAMES[NUM_LAYERS] = {
      "Walls",
      "Floor",
      "Ceiling",
  };

  Map() = default;

  int getMapCell(int x, int y, int layer) const;
  void setMapCell(int x, int y, int layer, int value);
  void fill(int layer, int value);
  size_t getWidth() const;
  size_t getHeight() const;
  void resize(size_t width, size_t height);

  void draw(sf::RenderTarget &target, float cellSize, int layer) const;
  void load(const std::filesystem::path &path);
  void save(const std::filesystem::path &path) const;

private:
  std::vector<std::vector<std::array<int, NUM_LAYERS>>> grid;
};

#endif // !_MAP_H
