#include "editor.h"
#include "ImGuiFileDialog.h"
#include "imgui-SFML.h"
#include "imgui.h"
#include "map.h"
#include "resources.h"
#include "thing.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstddef>

constexpr float CELL_SIZE = 48.0f;

Editor::Editor(sf::RenderWindow &window)
    : cell(), isFirstMouse(), view(window.getView()), isThingMode(),
      textureNo(), currentLayer(Map::LAYER_WALLS), blockmapDirty(true),
      selectedThing(), hoveredThing() {
  cell.setFillColor(sf::Color::Transparent);
  cell.setOutlineColor(sf::Color::Green);
  cell.setOutlineThickness(-2.5f);

  cell.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
}

void Editor::run(sf::RenderWindow &window, Map &map) {
  if (blockmapDirty) { rebuildBlockmap(map); }

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

  map.draw(window, CELL_SIZE, currentLayer);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
    map.draw(window, CELL_SIZE, Map::LAYER_WALLS, 180);
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        ImGuiFileDialog::Instance()->OpenDialog("OpenDialog", "Open", ".map");
      }

      if (ImGui::MenuItem("Save")) {
        if (savedFileName.empty()) {
          ImGuiFileDialog::Instance()->OpenDialog("SaveDialog", "Save", ".map");
        } else {
          map.save(savedFileName);
        }
      }

      if (ImGui::MenuItem("Save As")) {
        ImGuiFileDialog::Instance()->OpenDialog("SaveDialog", "Save As",
                                                ".map");
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }

  if (ImGuiFileDialog::Instance()->Display("SaveDialog")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      savedFileName = ImGuiFileDialog::Instance()->GetFilePathName();
      map.save(savedFileName);
    }

    ImGuiFileDialog::Instance()->Close();
  }

  if (ImGuiFileDialog::Instance()->Display("OpenDialog")) {
    if (ImGuiFileDialog::Instance()->IsOk()) {
      savedFileName = ImGuiFileDialog::Instance()->GetFilePathName();
      map.load(savedFileName);
    }

    ImGuiFileDialog::Instance()->Close();
  }

  ImGui::Begin("Editing Options");
  ImGui::Text("Editing mode: ");
  if (ImGui::Selectable("Grid", !isThingMode)) { isThingMode = false; }
  if (ImGui::Selectable("Things", isThingMode)) { isThingMode = true; }
  ImGui::Separator();

  ImGui::Text("Layer: ");
  if (ImGui::BeginCombo("##layer", Map::LAYER_NAMES[currentLayer])) {
    for (size_t i = 0; i < Map::NUM_LAYERS; i++) {
      if (ImGui::Selectable(Map::LAYER_NAMES[i], currentLayer == i)) {
        currentLayer = i;
      }
    }

    ImGui::EndCombo();
  }

  ImGui::Separator();

  if (isThingMode) {
    runThingEditor(window, map);
  } else {
    runGridEditor(window, map);
  }

  static int newSize[2];
  if (ImGui::Button("Resize")) {
    newSize[0] = map.getWidth();
    newSize[1] = map.getHeight();
    ImGui::OpenPopup("Resize");
  }

  if (ImGui::BeginPopupModal("Resize")) {
    ImGui::Text("New Size:");
    ImGui::InputInt2("##newSize", newSize);
    newSize[0] = std::max(0, newSize[0]);
    newSize[1] = std::max(0, newSize[1]);

    if (ImGui::Button("OK")) {
      map.resize(newSize[0], newSize[1]);
      ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Cancel")) { ImGui::CloseCurrentPopup(); }

    ImGui::EndPopup();
  }

  ImGui::End();
  window.setView(view);
}

void Editor::handleEvent(const sf::Event &event) {
  if (event.type == sf::Event::MouseWheelScrolled) {
    float zoom = 1.0f - 0.1f * event.mouseWheelScroll.delta;
    view.zoom(zoom);
  }
}

void Editor::runGridEditor(sf::RenderWindow &window, Map &map) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos) / CELL_SIZE;
    sf::Vector2i mapPos = static_cast<sf::Vector2i>(worldPos);

    cell.setPosition((sf::Vector2f)mapPos * CELL_SIZE);
    window.draw(cell);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
      map.setMapCell(
          mapPos.x, mapPos.y, currentLayer,
          sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? 0 : textureNo + 1);
    }
  }

  ImGui::Text("Texture No.: ");
  ImGui::InputInt("##tex_no", &textureNo);

  int textureSize = Resources::textures.getSize().y;
  ImGui::Text("Preview: ");

  sf::IntRect rect{textureNo * textureSize, 0, textureSize, textureSize};
  sf::Sprite preview{Resources::textures, rect};
  ImGui::Image(preview, sf::Vector2f(128.f, 128.f));

  if (ImGui::Button("Fill")) { map.fill(currentLayer, textureNo + 1); }

  ImGui::SameLine();
  if (ImGui::Button("Clear")) { map.fill(currentLayer, 0); }
}

void Editor::runThingEditor(sf::RenderWindow &window, Map &map) {
  if (!ImGui::GetIO().WantCaptureMouse) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(mousePos) / CELL_SIZE;
    sf::Vector2i mapPos = static_cast<sf::Vector2i>(worldPos);

    bool pressed = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    if (pressed) { selectedThing = nullptr; }

    hoveredThing = nullptr;
    if (mapPos.x >= 0 && mapPos.x < map.getWidth() && mapPos.y >= 0 &&
        mapPos.y < map.getHeight()) {
      for (const auto &thing : blockmap[mapPos.y][mapPos.x]) {
        const auto &def = thingDefs[thing->idx];
        float thingSize = def.size == 0.f ? .5f : def.size;

        sf::Vector2f halfSize = {thingSize / 2.f, thingSize / 2.f};
        sf::Vector2f start = thing->position - halfSize;
        sf::Vector2f end = thing->position + halfSize;

        if (worldPos.x >= start.x && worldPos.x <= end.x &&
            worldPos.y >= start.y && worldPos.y <= end.y) {
          hoveredThing = thing;
          if (pressed) { selectedThing = thing; }
        }
      }
    }
  }

  sf::RectangleShape rect, line;
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineThickness(1.5f);

  for (const auto &thing : map.getThings()) {
    const auto &def = thingDefs[thing.idx];
    float thingSize = def.size == 0.f ? .5f : def.size;

    line.setSize({thingSize * CELL_SIZE * 1.25f, 1.5f});
    line.setPosition(thing.position * CELL_SIZE);
    line.setRotation(thing.angle);

    rect.setSize(sf::Vector2f(thingSize, thingSize) * CELL_SIZE);
    rect.setOrigin(rect.getSize() / 2.f);
    rect.setPosition(thing.position * CELL_SIZE);

    sf::Color color = sf::Color::Blue;
    if (def.size == 0.f) { color = sf::Color::Red; }
    if (&thing == hoveredThing) { color = sf::Color::Yellow; }
    if (&thing == selectedThing) { color = sf::Color::Green; }

    rect.setOutlineColor(color);
    line.setFillColor(color);

    window.draw(rect);
    window.draw(line);

    if (def.texture >= 0) {
      int texSize = Resources::sprites.getSize().y;
      sf::IntRect rect{texSize * def.texture, 0, texSize, texSize};
      sf::Sprite sprite{Resources::sprites, rect};

      sf::Vector2f size{thingSize * CELL_SIZE, thingSize * CELL_SIZE};

      sprite.setScale(size / (float)texSize);
      sprite.setPosition(thing.position * CELL_SIZE - size / 2.f);

      window.draw(sprite);
    }
  }

  if (selectedThing) {
    sf::Vector2f oldPos = selectedThing->position;

    int idx = selectedThing->idx;
    float position[2];
    position[0] = selectedThing->position.x;
    position[1] = selectedThing->position.y;

    ImGui::Text("Type Index: ");
    ImGui::InputInt("##idx", &idx);

    ImGui::Text("Position: ");
    ImGui::DragFloat2("##pos", position, .1f);

    ImGui::Text("Angle: ");
    ImGui::DragFloat("##rot", &selectedThing->angle, 2.f);

    selectedThing->idx = std::clamp(idx, 0, (int)thingDefs.size() - 1);
    selectedThing->position.x = position[0];
    selectedThing->position.y = position[1];

    if (oldPos != selectedThing->position) { blockmapDirty = true; }
  }
}

void Editor::rebuildBlockmap(Map &map) {
  blockmap.clear();

  size_t w = map.getWidth(), h = map.getHeight();
  blockmap = std::vector(h, std::vector(w, std::set<MapThing *>()));

  for (auto &thing : map.getThings()) {
    const auto &def = thingDefs[thing.idx];
    float thingSize = def.size == 0.f ? .5f : def.size;

    sf::Vector2f halfSize = {thingSize / 2.f, thingSize / 2.f};
    sf::Vector2i start = static_cast<sf::Vector2i>(thing.position - halfSize);
    sf::Vector2i end = static_cast<sf::Vector2i>(thing.position + halfSize);

    for (int y = start.y; y <= end.y; y++) {
      for (int x = start.x; x <= end.x; x++) {
        if (y >= 0 && y < h && x >= 0 && x < w) {
          blockmap[y][x].insert(&thing);
        }
      }
    }
  }

  blockmapDirty = false;
}
