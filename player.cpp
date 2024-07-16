#include "player.h"
#include "map.h"
#include "raycast.h"
#include "resources.h"
#include "sound.h"
#include "state.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <cstdio>

constexpr float PI = 3.141592653589793f;
constexpr float TURN_SPEED = 150.f;
constexpr float MOUSE_TURN_SPEED = .08f;
constexpr float MOVE_SPEED = 2.5f;

Player::Player(Thing *thing) : thing(thing) {}

void Player::update(float dt, GameState state,
                    Animator<sf::Texture *> &animator,
                    std::optional<sf::Vector2i> mouseDelta, bool ghostmode) {
  if (mouseDelta) {
    thing->angle += mouseDelta->x * MOUSE_TURN_SPEED;
  } else {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
      thing->angle -= TURN_SPEED * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
      thing->angle += TURN_SPEED * dt;
    }
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
    thing->position += move * MOVE_SPEED * dt;
  } else {
    thing->move(state.map, move * MOVE_SPEED * dt);

    static float fireTimer = 0.f;

    fireTimer -= dt;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && fireTimer <= 0.f) {
      RayHit hit = raycast(state.map, thing->position, front, 64, true, thing);
      if (hit.thing) { hit.thing->damage(10.f, state); }

      sound::play(Resources::weaponSound);
      animator.setAnim(0);
      fireTimer = 1.f;
    }
  }

  sf::Listener::setUpVector(0.f, 0.f, -1.f);
  sf::Listener::setDirection(dx, dy, 0.f);
  sf::Listener::setPosition(thing->position.x, thing->position.y, 0.f);
}
