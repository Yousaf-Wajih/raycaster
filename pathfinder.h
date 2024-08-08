#pragma once

#include <SFML/System/Vector2.hpp>
#include <map>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "map.h"

class PathFinder {
public:
  using Node = std::pair<int, int>;

  PathFinder(const Map &map);
  const auto &getGraph() { return graph; }

  std::vector<Node> getPath(sf::Vector2i start_pos, sf::Vector2i goal_pos);

private:
  std::map<Node, std::optional<Node>> bfs(Node start, Node goal);

  std::map<Node, std::vector<Node>> graph;
};
