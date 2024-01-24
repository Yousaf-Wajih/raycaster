#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>
#include <string>

#include "editor.h"
#include "imgui-SFML.h"
#include "map.h"
#include "player.h"
#include "renderer.h"
#include "resources.h"

int main() {
  sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Raycaster",
                          sf::Style::Close | sf::Style::Titlebar);
  window.setVerticalSyncEnabled(true);

  if (!ImGui::SFML::Init(window)) {
    std::cerr << "Failed to init ImGui\n";
    return 1;
  }

  Map map{48.0f};

  if (!Resources::wallTexture.loadFromFile("wall_textures.png")) {
    std::cerr << "Failed to load wall_textures.png!\n";
  }

  Player player{};
  player.position = sf::Vector2f(50, 50);

  Renderer renderer{};
  renderer.init();

  Editor editor{};
  editor.init(window);

  enum class State { Editor, Game } state = State::Game;

  sf::Clock gameClock;
  while (window.isOpen()) {
    sf::Time deltaTime = gameClock.restart();
    ImGui::SFML::Update(window, deltaTime);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed &&
                 event.key.code == sf::Keyboard::Escape) {
        state = state == State::Game ? State::Editor : State::Game;
      }

      if (state == State::Editor) {
        editor.handleEvent(event);
      }

      ImGui::SFML::ProcessEvent(window, event);
    }

    window.clear();
    if (state == State::Game) {
      window.setView(window.getDefaultView());
      player.update(deltaTime.asSeconds());
      renderer.draw3dView(window, player, map);
    } else {
      editor.run(window, map);
    }

    ImGui::SFML::Render(window);
    window.display();

    window.setTitle("Raycaster | " +
                    std::to_string(1.0f / deltaTime.asSeconds()));
  }

  ImGui::SFML::Shutdown();
}
