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
constexpr float PLAYER_HALF_SIZE = 0.45f;

void Player::draw(sf::RenderTarget &target, float cellSize) {
  float size = PLAYER_HALF_SIZE * cellSize;
  sf::RectangleShape player(sf::Vector2f{size, size} * 2.f);
  player.setOrigin(size, size);
  player.setPosition(position * cellSize);
  player.setFillColor(sf::Color::Yellow);

  sf::RectangleShape line(sf::Vector2f(size * 3.f, cellSize / 30.f));
  line.setPosition(position * cellSize);
  line.setRotation(angle);
  line.setFillColor(sf::Color::Yellow);

  target.draw(line);
  target.draw(player);
}

void Player::update(float deltaTime, const Map &map) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    angle -= TURN_SPEED * deltaTime;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    angle += TURN_SPEED * deltaTime;
  }

  float radians = angle * PI / 180.0f;
  sf::Vector2f move{};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    move.x += cos(radians);
    move.y += sin(radians);
  }

  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    move.x -= cos(radians);
    move.y -= sin(radians);
  }

  float xOffset = move.x > 0.f ? PLAYER_HALF_SIZE : -PLAYER_HALF_SIZE;
  float yOffset = move.y > 0.f ? PLAYER_HALF_SIZE : -PLAYER_HALF_SIZE;
  move *= MOVE_SPEED * deltaTime;
  if (!checkMapCollision(map, {position.x + move.x + xOffset, position.y},
                         true)) {
    position.x += move.x;
  }

  if (!checkMapCollision(map, {position.x, position.y + move.y + yOffset},
                         false)) {
    position.y += move.y;
  }
}

bool Player::checkMapCollision(const Map &map, sf::Vector2f newPosition,
                               bool xAxis) {
  sf::Vector2f size = {PLAYER_HALF_SIZE, PLAYER_HALF_SIZE};
  sf::Vector2i start = static_cast<sf::Vector2i>(newPosition - size);
  sf::Vector2i end = static_cast<sf::Vector2i>(newPosition + size);

  if (xAxis) {
    for (int y = start.y; y <= end.y; y++) {
      if (map.getMapCell(newPosition.x, y, Map::LAYER_WALLS)) {
        return true;
      }
    }
  } else {
    for (int x = start.x; x <= end.x; x++) {
      if (map.getMapCell(x, newPosition.y, Map::LAYER_WALLS)) {
        return true;
      }
    }
  }

  return false;
}
