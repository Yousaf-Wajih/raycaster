#include "map.h"
#include "resources.h"
#include "thing.h"

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
#include <cstdio>
#include <fstream>
#include <ios>
#include <iostream>
#include <set>
#include <vector>

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

size_t Map::getWidth() const { return grid.empty() ? 0 : grid[0].size(); }
size_t Map::getHeight() const { return grid.size(); }

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

void Map::fill(int layer, int value) {
  if (layer < NUM_LAYERS) {
    for (auto &column : grid) {
      for (auto &cell : column) { cell[layer] = value; }
    }
  }
}

void Map::resize(size_t width, size_t height) {
  grid.resize(height);
  blockmap.resize(height);
  for (size_t i = 0; i < height; i++) {
    grid[i].resize(width);
    blockmap[i].resize(width);
  }
}

void Map::insertInBlockmap(int x, int y, Thing *thing) {
  if (y >= 0 && y < blockmap.size() && x >= 0 && x < blockmap[y].size()) {
    blockmap[y][x].insert(thing);
  }
}

void Map::removeFromBlockmap(int x, int y, Thing *thing) {
  if (y >= 0 && y < blockmap.size() && x >= 0 && x < blockmap[y].size()) {
    blockmap[y][x].erase(thing);
  }
}

std::set<Thing *> Map::getBlockmap(int x, int y) const {
  if (y >= 0 && y < blockmap.size() && x >= 0 && x < blockmap[y].size()) {
    return blockmap[y][x];
  } else {
    return {};
  }
}

void Map::load(const std::filesystem::path &path) {
  std::ifstream in{path, std::ios::in | std::ios::binary};
  if (!in.is_open()) {
    std::cerr << "Failed to open file \"" << path << "\" for input\n";
  }

  size_t w, h;
  in.read(reinterpret_cast<char *>(&w), sizeof w);
  in.read(reinterpret_cast<char *>(&h), sizeof h);

  grid = std::vector(h, std::vector(w, std::array<int, NUM_LAYERS>()));
  blockmap = std::vector(h, std::vector(w, std::set<Thing *>()));
  for (size_t y = 0; y < grid.size(); y++) {
    for (size_t x = 0; x < grid[y].size(); x++) {
      in.read(reinterpret_cast<char *>(grid[y][x].data()),
              sizeof grid[y][x][0] * NUM_LAYERS);
    }
  }

  size_t num_things;
  in.read(reinterpret_cast<char *>(&num_things), sizeof num_things);

  things.resize(num_things);
  for (size_t i = 0; i < things.size(); i++) {
    in.read(reinterpret_cast<char *>(&things[i]), sizeof things[i]);
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
  out.write(reinterpret_cast<const char *>(&w), sizeof w);
  out.write(reinterpret_cast<const char *>(&h), sizeof h);

  for (size_t y = 0; y < grid.size(); y++) {
    for (size_t x = 0; x < grid[y].size(); x++) {
      out.write(reinterpret_cast<const char *>(grid[y][x].data()),
                sizeof grid[y][x][0] * NUM_LAYERS);
    }
  }

  std::vector<MapThing> things = {
      {0, {2.2f, 2.2f}, 45.f},
      {1, {6.9f, 5.8f}, 0.f},
      {2, {6.9f, 9.8f}, 0.f},
      {3, {6.9f, 7.8f}, 0.f},
  };

  size_t num_things = things.size();
  out.write(reinterpret_cast<const char *>(&num_things), sizeof num_things);

  for (size_t i = 0; i < things.size(); i++) {
    out.write(reinterpret_cast<const char *>(&things[i]), sizeof things[i]);
  }
}

const std::vector<MapThing> &Map::getThings() const { return things; }
