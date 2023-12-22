#ifndef _MAP_H
#define _MAP_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <string>
#include <vector>

class Map {
public:
  Map(float cellSize, int width, int height);
  Map(float cellSize, const std::string &filename);

  void draw(sf::RenderTarget &target);

  const std::vector<std::vector<sf::Color>> &getGrid() const;
  float getCellSize() const;

private:
  std::vector<std::vector<sf::Color>> grid;
  float cellSize;
};

#endif // !_MAP_H
