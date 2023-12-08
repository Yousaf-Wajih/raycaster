#include "map.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

Map::Map(float cellSize, int width, int height)
    : cellSize(cellSize), grid(height, std::vector(width, 0)) {}

void Map::draw(sf::RenderTarget &target) {
  if (grid.empty()) {
    return;
  }

  sf::RectangleShape background(sf::Vector2f((float)grid[0].size() * cellSize,
                                             (float)grid.size() * cellSize));
  background.setFillColor(sf::Color::White);
  target.draw(background);

  sf::RectangleShape cell(sf::Vector2f(cellSize * 0.95f, cellSize * 0.95f));
}
