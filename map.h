#ifndef _MAP_H
#define _MAP_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <vector>

class Map {
public:
  Map(float cellSize, int width, int height);
  void draw(sf::RenderTarget &target);

private:
  std::vector<std::vector<int>> grid;
  float cellSize;
};

#endif // !_MAP_H
