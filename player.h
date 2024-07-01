#ifndef _PLAYER_H
#define _PLAYER_H

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Window.hpp>
#include <optional>

#include "map.h"
#include "thing.h"

class Player {
public:
  Player(Thing *thing);
  void update(float deltaTime, Map &map,
              std::optional<sf::Vector2i> mouseDelta = std::nullopt,
              bool ghostmode = false);

  Thing *thing;
};

#endif // !_PLAYER_H
