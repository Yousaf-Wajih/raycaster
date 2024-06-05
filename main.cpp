#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <iostream>
#include <string>
#include <vector>

#include "editor.h"
#include "imgui-SFML.h"
#include "map.h"
#include "player.h"
#include "renderer.h"
#include "resources.h"
#include "sprite.h"

int main(int argc, const char **argv) {
  sf::RenderWindow window(sf::VideoMode(SCREEN_W, SCREEN_H), "Raycaster",
                          sf::Style::Close | sf::Style::Titlebar);
  window.setVerticalSyncEnabled(true);

  if (!ImGui::SFML::Init(window)) {
    std::cerr << "Failed to init ImGui\n";
    return 1;
  }

  if (!Resources::texturesImage.loadFromFile("textures.png")) {
    std::cerr << "Failed to load textures.png!\n";
  }
  Resources::textures.loadFromImage(Resources::texturesImage);

  if (!Resources::sprites.loadFromFile("sprites.png")) {
    std::cerr << "Failed to load sprites.png!\n";
  }

  Player player{};
  player.position = sf::Vector2f(2.2f, 2.2f);

  Renderer renderer{};
  renderer.init();

  Editor editor{};
  editor.init(window);

  Map map{};
  if (argc > 1) {
    editor.savedFileName = argv[1];
    map.load(editor.savedFileName);
  }

  std::vector<Sprite> sprites = {
      {{4.5f, 6.5f}, 0},
      {{4.5f, 9.5f}, 1},
      {{4.5f, 12.5f}, 2},
  };

  enum class State { Editor, Game } state = State::Game;
  bool view2d = false;
  float gridSize2d = 64.f;

  sf::Clock gameClock;
  while (window.isOpen()) {
    sf::Time deltaTime = gameClock.restart();
    ImGui::SFML::Update(window, deltaTime);

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Escape) {
          state = state == State::Game ? State::Editor : State::Game;
        }

        if (event.key.code == sf::Keyboard::Tab) { view2d = !view2d; }
      }

      if (state == State::Editor) { editor.handleEvent(event); }

      ImGui::SFML::ProcessEvent(window, event);
    }

    window.clear();
    if (state == State::Game) {
      player.update(deltaTime.asSeconds(), map);
      if (view2d) {
        sf::View view = window.getDefaultView();
        view.setCenter(player.position * gridSize2d);
        window.setView(view);
        map.draw(window, gridSize2d, Map::LAYER_WALLS);
        player.draw(window, gridSize2d);
      } else {
        window.setView(window.getDefaultView());
        renderer.draw3dView(window, player, map, sprites);
      }
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
