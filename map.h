#ifndef _MAP_H
#define _MAP_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <filesystem>
#include <string>
#include <vector>

typedef std::vector<std::vector<int>> MapGrid;

class Map {
public:
  Map(float cellSize);
  Map(float cellSize, int width, int height);
  Map(float cellSize, const std::string &filename);

  void draw(sf::RenderTarget &target);
  void setMapCell(int x, int y, int value);

  void load(const std::filesystem::path &path);
  void save(const std::filesystem::path &path);

  const MapGrid &getGrid() const;
  float getCellSize() const;

private:
  MapGrid grid;
  float cellSize;
};

#endif // !_MAP_H
