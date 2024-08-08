#pragma once

class Game;
class Map;
class PathFinder;

struct GameState {
  Game &game;
  Map &map;
  PathFinder &pathfinder;
  float dt;
};
