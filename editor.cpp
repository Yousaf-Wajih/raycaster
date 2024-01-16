#include "editor.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

void Editor::init(sf::RenderWindow &window) { view = window.getView(); }

void Editor::run(sf::RenderWindow &window) {
  if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
    if (isFirstMouse) {
      lastMousePos = sf::Mouse::getPosition(window);
      isFirstMouse = false;
    } else {
      sf::Vector2i mousePos = sf::Mouse::getPosition(window);
      sf::Vector2i mouseDelta = mousePos - lastMousePos;

      view.setCenter(view.getCenter() - (sf::Vector2f)mouseDelta);
      sf::Mouse::setPosition(lastMousePos, window);
    }

    window.setMouseCursorVisible(false);
  } else {
    isFirstMouse = true;
    window.setMouseCursorVisible(true);
  }

  window.setView(view);
}

void Editor::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::MouseWheelScrolled) {
    float zoom = 1.0f - 0.1f * event.mouseWheelScroll.delta;
    view.zoom(zoom);
  }
}
