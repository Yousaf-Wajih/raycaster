#include "pathfinder.h"
#include "map.h"

#include <SFML/System/Vector2.hpp>
#include <array>
#include <deque>
#include <optional>
#include <utility>
#include <vector>

constexpr std::array<PathFinder::Node, 8> directions{{
    {-1, 0},
    {0, -1},
    {1, 0},
    {0, 1},
    {-1, -1},
    {1, -1},
    {1, 1},
    {-1, 1},
}};

PathFinder::PathFinder(const Map &map) {
  for (int y = 0; y < map.getHeight(); y++) {
    for (int x = 0; x < map.getWidth(); x++) {
      int cell = map.getMapCell(x, y, Map::LAYER_WALLS);
      if (!cell) {
        auto it = graph.find({x, y});
        std::vector<Node> nodes{};
        if (it != graph.end()) nodes = it->second;

        for (const auto &[dx, dy] : directions) {
          if (!map.getMapCell(x + dx, y + dy, Map::LAYER_WALLS)) {
            nodes.push_back({x + dx, y + dy});
          }
        }

        graph[{x, y}] = nodes;
      }
    }
  }
}

std::vector<PathFinder::Node> PathFinder::getPath(sf::Vector2i start_pos,
                                                  sf::Vector2i goal_pos) {
  Node start = {start_pos.x, start_pos.y};
  Node goal = {goal_pos.x, goal_pos.y};

  auto visited = bfs(start, goal);
  std::vector<Node> path{goal};

  std::optional<Node> step = start;
  if (visited.count(goal)) step = visited.at(goal);

  while (step && step != start) {
    path.push_back(*step);
    step = visited[*step];
  }

  return path;
}

std::map<PathFinder::Node, std::optional<PathFinder::Node>>
PathFinder::bfs(Node start, Node goal) {
  std::map<Node, std::optional<Node>> visited{
      {start, std::nullopt},
  };

  std::deque<Node> queue{start};
  while (!queue.empty()) {
    auto currentNode = queue.front();
    queue.pop_front();

    if (currentNode == goal) break;

    auto children = graph[currentNode];
    for (const auto &nextNode : children) {
      if (!visited.count(nextNode)) {
        visited[nextNode] = currentNode;
        queue.push_back(nextNode);
      }
    }
  }

  return visited;
}
