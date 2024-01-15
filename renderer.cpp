#include "renderer.h"
#include "player.h"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>

constexpr float PI = 3.141592653589793f;
constexpr float PLAYER_FOV = 60.0f;
constexpr float CAMERA_Z = 0.5f * SCREEN_H;
constexpr size_t MAX_RAYCAST_DEPTH = 64;

void Renderer::init() {
  floorBuffer.create(SCREEN_W, SCREEN_H);
  floorBufferSprite.setTexture(floorBuffer);

  if (!skyTexture.loadFromFile("sky_texture.png")) {
    std::cerr << "Failed to load sky_texture.png!\n";
  }
  skyTexture.setRepeated(true);

  if (!wallTexture.loadFromFile("wall_texture.png")) {
    std::cerr << "Failed to load wall_texture.png!\n";
  }

  if (wallTexture.getSize().x != wallTexture.getSize().y) {
    std::cerr << "ERROR: Texture wall_texture.png is not square\n";
  }

  if (!floorImage.loadFromFile("floor_texture.png")) {
    std::cerr << "Failed to load floor_texture.png!\n";
  }

  if (floorImage.getSize().x != floorImage.getSize().y) {
    std::cerr << "ERROR: Texture floor_texture.png is not square\n";
  }
}

void Renderer::draw3dView(sf::RenderTarget &target, const Player &player,
                          const Map &map) {
  float radians = player.angle * PI / 180.0f;
  sf::Vector2f direction{std::cos(radians), std::sin(radians)};
  sf::Vector2f plane{-direction.y, direction.x * 0.66f};
  sf::Vector2f position = player.position / map.getCellSize();

  int xOffset = SCREEN_W / PLAYER_TURN_SPEED * player.angle;
  while (xOffset < 0) {
    xOffset += skyTexture.getSize().x;
  }
  sf::Vertex sky[] = {
      sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(xOffset, 0.0f)),
      sf::Vertex(sf::Vector2f(0.0f, SCREEN_H),
                 sf::Vector2f(xOffset, skyTexture.getSize().y)),
      sf::Vertex(sf::Vector2f(SCREEN_W, SCREEN_H),
                 sf::Vector2f(xOffset + skyTexture.getSize().x,
                              skyTexture.getSize().y)),
      sf::Vertex(sf::Vector2f(SCREEN_W, 0.0f),
                 sf::Vector2f(xOffset + skyTexture.getSize().x, 0.0f)),
  };
  target.draw(sky, 4, sf::Quads, sf::RenderStates(&skyTexture));

  uint8_t floorPixels[(size_t)SCREEN_W * (size_t)SCREEN_H * 4]{};
  for (size_t y = SCREEN_H / 2; y < SCREEN_H; y++) {
    sf::Vector2f rayDirLeft{direction - plane}, rayDirRight{direction + plane};
    float rowDistance = CAMERA_Z / ((float)y - SCREEN_H / 2);

    sf::Vector2f floorStep =
        rowDistance * (rayDirRight - rayDirLeft) / SCREEN_W;
    sf::Vector2f floor = position + rowDistance * rayDirLeft;

    for (size_t x = 0; x < SCREEN_W; x++) {
      sf::Vector2i cell{floor};

      float textureSize = floorImage.getSize().x;
      sf::Vector2i texCoords{textureSize * (floor - (sf::Vector2f)cell)};
      texCoords.x &= (int)textureSize - 1;
      texCoords.y &= (int)textureSize - 1;

      sf::Color color = floorImage.getPixel(texCoords.x, texCoords.y);
      floorPixels[(x + y * (size_t)SCREEN_W) * 4 + 0] = color.r;
      floorPixels[(x + y * (size_t)SCREEN_W) * 4 + 1] = color.g;
      floorPixels[(x + y * (size_t)SCREEN_W) * 4 + 2] = color.b;
      floorPixels[(x + y * (size_t)SCREEN_W) * 4 + 3] = color.a;

      floor += floorStep;
    }
  }

  floorBuffer.update(floorPixels);
  target.draw(floorBufferSprite);

  sf::VertexArray walls{sf::Lines};
  for (size_t i = 0; i < SCREEN_W; i++) {
    float cameraX = i * 2.0f / SCREEN_W - 1.0f; // -1.0f -> 0.0f -> 1.0f
    sf::Vector2f rayPos = position;
    sf::Vector2f rayDir = direction + plane * cameraX;

    sf::Vector2f deltaDist{
        std::abs(1.0f / rayDir.x),
        std::abs(1.0f / rayDir.y),
    };

    sf::Vector2i mapPos{rayPos};
    sf::Vector2i step;
    sf::Vector2f sideDist;

    if (rayDir.x < 0.0f) {
      step.x = -1;
      sideDist.x = (-mapPos.x + rayPos.x) * deltaDist.x;
    } else {
      step.x = 1;
      sideDist.x = (mapPos.x - rayPos.x + 1.0f) * deltaDist.x;
    }

    if (rayDir.y < 0.0f) {
      step.y = -1;
      sideDist.y = (-mapPos.y + rayPos.y) * deltaDist.y;
    } else {
      step.y = 1;
      sideDist.y = (mapPos.y - rayPos.y + 1.0f) * deltaDist.y;
    }

    bool didHit{}, isHitVertical{};
    size_t depth = 0;
    while (!didHit && depth < MAX_RAYCAST_DEPTH) {
      if (sideDist.x < sideDist.y) {
        sideDist.x += deltaDist.x;
        mapPos.x += step.x;
        isHitVertical = false;
      } else {
        sideDist.y += deltaDist.y;
        mapPos.y += step.y;
        isHitVertical = true;
      }

      int x = mapPos.x, y = mapPos.y;
      const auto &grid = map.getGrid();

      if (y >= 0 && y < grid.size() && x >= 0 && x < grid[y].size() &&
          grid[y][x]) {
        didHit = true;
      }

      depth++;
    }

    if (didHit) {
      float perpWallDist =
          isHitVertical ? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x;
      float wallHeight = SCREEN_H / perpWallDist;

      float wallStart = (-wallHeight + SCREEN_H) / 2.0f;
      float wallEnd = (wallHeight + SCREEN_H) / 2.0f;

      float textureSize = wallTexture.getSize().x;

      float wallX = isHitVertical ? rayPos.x + perpWallDist * rayDir.x
                                  : rayPos.y + perpWallDist * rayDir.y;
      wallX -= std::floor(wallX);
      float textureX = wallX * textureSize;

      float brightness = 1.0f - (perpWallDist / (float)MAX_RAYCAST_DEPTH);
      if (isHitVertical) {
        brightness *= 0.7f;
      }

      sf::Color color =
          sf::Color(255 * brightness, 255 * brightness, 255 * brightness);

      walls.append(sf::Vertex(sf::Vector2f((float)i, wallStart), color,
                              sf::Vector2f(textureX, 0.0f)));
      walls.append(sf::Vertex(sf::Vector2f((float)i, wallEnd), color,
                              sf::Vector2f(textureX, textureSize)));
    }
  }

  sf::RenderStates states{&wallTexture};
  target.draw(walls, states);
}
