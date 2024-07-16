#pragma once

class Game;
class Map;

struct GameState {
  Game &game;
  Map &map;
  float dt;
};
