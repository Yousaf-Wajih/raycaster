#include "player.h"
#include "map.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

constexpr float PI = 3.141592653589793f;
constexpr float TURN_SPEED = PLAYER_TURN_SPEED;
constexpr float MOVE_SPEED = 2.5f;

Player::Player(Thing *thing) : thing(thing) {}

void Player::update(float deltaTime, Map &map, bool ghostmode) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    thing->angle -= TURN_SPEED * deltaTime;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    thing->angle += TURN_SPEED * deltaTime;
  }

  float radians = thing->angle * PI / 180.0f;
  float dx = std::cos(radians), dy = std::sin(radians);
  sf::Vector2f front = {dx, dy}, right = {-dy, dx};

  sf::Vector2f move{};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { move += front; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { move -= right; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { move -= front; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { move += right; }

  if (ghostmode) {
    thing->position += move * MOVE_SPEED * deltaTime;
  } else {
    thing->move(map, move * MOVE_SPEED * deltaTime);
  }
}
