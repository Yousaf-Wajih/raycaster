#include "map.h"
#include "resources.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

void Map::draw(sf::RenderTarget &target, float cellSize, int layer,
               uint8_t alpha) const {
  if (grid.empty()) { return; }

  int textureSize = Resources::textures.getSize().y;
  sf::Vector2f size{cellSize * 0.95f, cellSize * 0.95f};
  sf::Sprite sprite{Resources::textures,
                    sf::IntRect(0, 0, textureSize, textureSize)};
  sprite.setScale(size / (float)textureSize);
  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, cellSize * 0.95f));

  for (size_t y = 0; y < grid.size(); y++) {
    for (size_t x = 0; x < grid[y].size(); x++) {
      if (grid[y][x][layer] > 0) {
        sprite.setTextureRect(sf::IntRect((grid[y][x][layer] - 1) * textureSize,
                                          0, textureSize, textureSize));
        sprite.setPosition(sf::Vector2f(x, y) * cellSize +
                           sf::Vector2f(cellSize * 0.025f, cellSize * 0.025f));
        sprite.setColor(sf::Color(255, 255, 255, alpha));
        target.draw(sprite);
      } else if (alpha == 255) {
        cell.setFillColor(sf::Color(70, 70, 70));
        cell.setPosition(sf::Vector2f(x, y) * cellSize +
                         sf::Vector2f(cellSize * 0.025f, cellSize * 0.025f));
        target.draw(cell);
      }
    }
  }
}

int Map::getMapCell(int x, int y, int layer) const {
  if (layer < NUM_LAYERS && y >= 0 && y < grid.size() && x >= 0 &&
      x < grid[y].size()) {
    return grid[y][x][layer];
  } else {
    return 0;
  }
}

void Map::setMapCell(int x, int y, int layer, int value) {
  if (layer < NUM_LAYERS && y >= 0 && y < grid.size() && x >= 0 &&
      x < grid[y].size()) {
    grid[y][x][layer] = value;
  }
}

void Map::load(const std::filesystem::path &path) {
  std::ifstream in{path, std::ios::in | std::ios::binary};
  if (!in.is_open()) {
    std::cerr << "Failed to open file \"" << path << "\" for input\n";
  }

  size_t w, h;
  in.read(reinterpret_cast<char *>(&w), sizeof(w));
  in.read(reinterpret_cast<char *>(&h), sizeof(h));

  grid = std::vector(h, std::vector(w, std::array<int, NUM_LAYERS>()));
  for (size_t y = 0; y < grid.size(); y++) {
    for (size_t x = 0; x < grid[y].size(); x++) {
      in.read(reinterpret_cast<char *>(grid[y][x].data()),
              sizeof(grid[y][x][0]) * NUM_LAYERS);
    }
  }
}

void Map::save(const std::filesystem::path &path) const {
  std::ofstream out{path, std::ios::out | std::ios::binary};
  if (!out.is_open()) {
    std::cerr << "Failed to open file \"" << path << "\" for output\n";
  }

  if (grid.empty()) { return; }

  size_t h = grid.size();
  size_t w = grid[0].size();
  out.write(reinterpret_cast<const char *>(&w), sizeof(w));
  out.write(reinterpret_cast<const char *>(&h), sizeof(h));

  for (size_t y = 0; y < grid.size(); y++) {
    for (size_t x = 0; x < grid[y].size(); x++) {
      out.write(reinterpret_cast<const char *>(grid[y][x].data()),
                sizeof(grid[y][x][0]) * NUM_LAYERS);
    }
  }
}

void Map::fill(int layer, int value) {
  if (layer < NUM_LAYERS) {
    for (auto &column : grid) {
      for (auto &cell : column) {
        cell[layer] = value;
      }
    }
  }
}

size_t Map::getWidth() const { return grid.empty() ? 0 : grid[0].size(); }
size_t Map::getHeight() const { return grid.size(); }

void Map::resize(size_t width, size_t height) {
  grid.resize(height);
  for (auto &column : grid) {
    column.resize(width);
  }
}
