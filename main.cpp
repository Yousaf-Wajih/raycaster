#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>

#include "map.h"
#include "player.h"
#include "renderer.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Raycaster",
                          sf::Style::Close | sf::Style::Titlebar);

  Map map(48.0f, "map.png");

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
    renderer.draw3dView(window, player, map);
    window.display();
  }
}
