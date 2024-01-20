#include "editor.h"
#include "imgui.h"
#include "map.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

void Editor::init(sf::RenderWindow &window) {
  view = window.getView();
  cell.setFillColor(sf::Color::Green);
}

void Editor::run(sf::RenderWindow &window, Map &map) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Save")) {
        map.save("test.map");
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  sf::Vector2i mousePos = sf::Mouse::getPosition(window);

  if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
    if (isFirstMouse) {
      lastMousePos = mousePos;
      isFirstMouse = false;
    } else {
      sf::Vector2i mouseDelta = mousePos - lastMousePos;
      view.setCenter(view.getCenter() - (sf::Vector2f)mouseDelta);
      sf::Mouse::setPosition(lastMousePos, window);
    }

    window.setMouseCursorVisible(false);
  } else {
    isFirstMouse = true;
    window.setMouseCursorVisible(true);
  }

  if (!ImGui::GetIO().WantCaptureMouse) {
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos);
    sf::Vector2i mapPos = (sf::Vector2i)(worldPos / map.getCellSize());
    cell.setSize(sf::Vector2f(map.getCellSize(), map.getCellSize()));
    cell.setPosition((sf::Vector2f)mapPos * map.getCellSize());
    window.draw(cell);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      map.setMapCell(mapPos.x, mapPos.y,
                     sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? 0 : 1);
    }
  }

  window.setView(view);
}

void Editor::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::MouseWheelScrolled) {
    float zoom = 1.0f - 0.1f * event.mouseWheelScroll.delta;
    view.zoom(zoom);
  }
}
