#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "map.h"
#include "player.h"
#include "renderer.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(1200, 675), "Raycaster");

  std::vector<std::vector<int>> grid = {
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
      {1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
      {1, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1},
      {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1},
      {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
      {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  Map map(48.0f, grid);

  Player player;
  player.position = sf::Vector2f(50, 50);

  Renderer renderer;

  sf::Clock gameClock;
  while (window.isOpen()) {
    float deltaTime = gameClock.restart().asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    player.update(deltaTime);

    window.clear();
    map.draw(window);
    renderer.drawRays(window, player, map);
    player.draw(window);
    window.display();
  }
}
