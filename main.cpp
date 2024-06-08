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
#include <vector>

#include "editor.h"
#include "imgui-SFML.h"
#include "map.h"
#include "player.h"
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

  Player player{};
  Renderer renderer{};
  Editor editor{window};
  Map map{};

  if (argc > 1) { map.load(editor.savedFileName = argv[1]); }
  player.position = sf::Vector2f(2.2f, 2.2f);

  std::vector<std::shared_ptr<Thing>> things = {
      std::make_shared<Thing>(sf::Vector2f{6.9f, 5.8f}, .5f, 0),
      std::make_shared<Thing>(sf::Vector2f{6.9f, 9.8f}, .5f, 1),
      std::make_shared<Thing>(sf::Vector2f{6.9f, 7.8f}, 0.f, 2),
  };

  for (auto &thing : things) {
    sf::Vector2f halfSize = {thing->size / 2.f, thing->size / 2.f};
    sf::Vector2i start = static_cast<sf::Vector2i>(thing->position - halfSize);
    sf::Vector2i end = static_cast<sf::Vector2i>(thing->position + halfSize);

    for (int y = start.y; y <= end.y; y++) {
      for (int x = start.x; x <= end.x; x++) {
        map.insertInBlockmap(x, y, thing.get());
      }
    }
  }

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

        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Transparent);
        rect.setOutlineColor(sf::Color::Green);
        rect.setOutlineThickness(2.f);

        for (const auto &thing : things) {
          rect.setSize(sf::Vector2f(thing->size, thing->size) * gridSize2d);
          rect.setOrigin(rect.getSize() / 2.f);
          rect.setPosition(thing->position * gridSize2d);
          window.draw(rect);
        }
      } else {
        window.setView(window.getDefaultView());
        renderer.draw3dView(window, player, map, things);
      }
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
