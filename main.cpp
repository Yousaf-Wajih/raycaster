#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

#include "map.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(1200, 675), "Raycaster");

  Map map(32.0f, 30, 20);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    window.clear();
    map.draw(window);
    window.display();
  }
}
