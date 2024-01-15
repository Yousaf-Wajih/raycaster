#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <string>

#include "map.h"
#include "player.h"
#include "renderer.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Raycaster",
                          sf::Style::Close | sf::Style::Titlebar);
  window.setVerticalSyncEnabled(true);

  Map map(48.0f, "map.png");

  Player player{};
  player.position = sf::Vector2f(50, 50);

  Renderer renderer{};
  renderer.init();

  enum class State { Editor, Game } state = State::Game;

  sf::Clock gameClock;
  while (window.isOpen()) {
    float deltaTime = gameClock.restart().asSeconds();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape) {
        state = state == State::Game ? State::Editor : State::Game;
      }
    }

    window.clear();
    if (state == State::Game) {
      player.update(deltaTime);
      renderer.draw3dView(window, player, map);
    } else {
      map.draw(window);
    }
    window.display();

    window.setTitle("Raycaster | " + std::to_string(1.0f / deltaTime));
  }
}
