#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
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

#include "editor.h"
#include "game.h"
#include "imgui-SFML.h"
#include "map.h"
#include "renderer.h"
#include "resources.h"
#include "thing.h"

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

  Map map{};
  Editor editor{window};
  if (argc > 1) { map.load(editor.savedFileName = argv[1]); }

  Game game{map};

  enum class State { Editor, Game } state = State::Game;
  bool view2d = false, game_mode = false;

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
        if (event.key.code == sf::Keyboard::Tilde) { game_mode = !game_mode; }
      }

      if (game_mode) { state = State::Game; }

      if (state == State::Editor) { editor.handleEvent(event); }

      ImGui::SFML::ProcessEvent(window, event);
    }

    window.clear();
    if (state == State::Game) {
      game.update(deltaTime.asSeconds(), map, game_mode);
      game.render(window, map, view2d, game_mode);
    } else {
      editor.run(window, map);
    }

    ImGui::SFML::Render(window);
    window.display();

    window.setTitle("Raycaster | " +
                    std::to_string(1.f / deltaTime.asSeconds()));
  }

  ImGui::SFML::Shutdown();
}
