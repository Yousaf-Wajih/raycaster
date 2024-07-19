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
#include <memory>
#include <string>

#include "editor.h"
#include "game.h"
#include "imgui-SFML.h"
#include "map.h"
#include "renderer.h"
#include "resources.h"
#include "thing.h"

int main(int argc, const char **argv) {
  sf::RenderWindow window(sf::VideoMode(960, 540), "Raycaster");
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

  if (!Resources::weaponSound.loadFromFile("weapon.wav")) {
    std::cerr << "Failed to load weapon.wav!\n";
  }

  Map map{};
  Editor editor{window};
  if (argc > 1) { map.load(editor.savedFileName = argv[1]); }

  std::unique_ptr game = std::make_unique<Game>(map);

  enum class State { Editor, Game } state = State::Game;
  bool view2d = false, game_mode = false;

  bool shouldResize = false, justResized = false;

  sf::Clock gameClock;
  while (window.isOpen()) {
    sf::Time deltaTime = gameClock.restart();
    ImGui::SFML::Update(window, deltaTime);

    justResized = false;
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
      case sf::Event::Closed: window.close(); break;
      case sf::Event::KeyPressed:
        switch (event.key.code) {
        case sf::Keyboard::Escape:
          state = state == State::Game ? State::Editor : State::Game;
          break;
        case sf::Keyboard::Tab:
          if (state == State::Game) { view2d = !view2d; }
          break;
        case sf::Keyboard::Tilde: game_mode = !game_mode; break;
        case sf::Keyboard::R: game = std::make_unique<Game>(map); break;
        default: break;
        }
        break;
      case sf::Event::Resized: {
        float aspect = (float)event.size.width / event.size.height;
        if (aspect < 1.f) { shouldResize = true; }
        justResized = true;
      }
      default: break;
      }

      if (shouldResize && !justResized) {
        window.setSize(sf::Vector2u(window.getSize().y, window.getSize().y));
        shouldResize = false;
      }

      if (game_mode) { state = State::Game; }
      if (state == State::Editor) {
        editor.handleEvent(event);
      } else {
        game->handleEvent(event, window);
      }

      ImGui::SFML::ProcessEvent(window, event);
    }

    window.clear();
    if (state == State::Game) {
      game->update(window, deltaTime.asSeconds(), map, game_mode);
      game->render(window, map, view2d, game_mode);
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
