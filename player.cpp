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
constexpr float PLAYER_HALF_SIZE = .2f;

void Player::draw(sf::RenderTarget &target, float cellSize) {
  float size = PLAYER_HALF_SIZE * cellSize;
  sf::RectangleShape rect(sf::Vector2f{size, size} * 2.f);
  rect.setOrigin(size, size);
  rect.setPosition(position * cellSize);
  rect.setFillColor(sf::Color::Yellow);

  sf::RectangleShape line(sf::Vector2f(size * 3.f, cellSize / 30.f));
  line.setPosition(position * cellSize);
  line.setRotation(angle);
  line.setFillColor(sf::Color::Yellow);

  target.draw(line);
  target.draw(rect);
}

void Player::update(float deltaTime, const Map &map) {
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
    angle -= TURN_SPEED * deltaTime;
  }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
    angle += TURN_SPEED * deltaTime;
  }

  float radians = angle * PI / 180.0f;
  float dx = std::cos(radians), dy = std::sin(radians);
  sf::Vector2f front = {dx, dy}, right = {-dy, dx};

  sf::Vector2f move{};
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) { move += front; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { move -= right; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) { move -= front; }
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) { move += right; }

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
  sf::Vector2f start = newPosition - size;
  sf::Vector2f end = newPosition + size;

  if (xAxis) {
    for (int y = start.y; y <= end.y; y++) {
      if (map.getMapCell(newPosition.x, y, Map::LAYER_WALLS)) { return true; }

      const auto &set = map.getBlockmap(newPosition.x, y);
      for (const auto &thing : set) {
        if (thing->size == 0.f) { continue; }

        sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
        sf::Vector2f thingStart = thing->position - halfSize;
        sf::Vector2f thingEnd = thing->position + halfSize;

        float px = newPosition.x;
        float py0 = start.y, py1 = end.y, magnitude = py1 - py0;
        float t1 = (thingStart.y - py0) / magnitude;
        float t2 = (thingEnd.y - py0) / magnitude;

        float entry = std::fmin(t1, t2);
        float exit = std::fmax(t1, t2);

        bool yIntersect = exit > entry && exit > 0.f && entry < 1.f;
        if (px >= thingStart.x && px <= thingEnd.x && yIntersect) {
          return true;
        }
      }
    }
  } else {
    for (int x = start.x; x <= end.x; x++) {
      if (map.getMapCell(x, newPosition.y, Map::LAYER_WALLS)) { return true; }

      const auto &set = map.getBlockmap(x, newPosition.y);
      for (const auto &thing : set) {
        if (thing->size == 0.f) { continue; }

        sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
        sf::Vector2f thingStart = thing->position - halfSize;
        sf::Vector2f thingEnd = thing->position + halfSize;

        float py = newPosition.y;
        float px0 = start.x, px1 = end.x, magnitude = px1 - px0;
        float t1 = (thingStart.x - px0) / magnitude;
        float t2 = (thingEnd.x - px0) / magnitude;

        float entry = std::fmin(t1, t2);
        float exit = std::fmax(t1, t2);

        bool xIntersect = exit > entry && exit > 0.f && entry < 1.f;
        if (py >= thingStart.y && py <= thingEnd.y && xIntersect) {
          return true;
        }
      }
    }
  }

  return false;
}
