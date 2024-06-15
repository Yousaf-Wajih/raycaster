#include "renderer.h"
#include "resources.h"
#include "thing.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

constexpr float ASPECT = SCREEN_W / SCREEN_H;
constexpr float PI = 3.141592653589793f;
constexpr float PLAYER_FOV = 60.0f;
constexpr float CAMERA_Z = 0.5f * SCREEN_H;
constexpr size_t MAX_RAYCAST_DEPTH = 64;

Renderer::Renderer() {
  screenBuffer.create(SCREEN_W, SCREEN_H);
  screenBufferSprite.setTexture(screenBuffer);

  if (!skyTexture.loadFromFile("sky_texture.png")) {
    std::cerr << "Failed to load sky_texture.png!\n";
  }
  skyTexture.setRepeated(true);
}

void Renderer::draw3dView(sf::RenderTarget &target, sf::Vector2f position,
                          float angle, const Map &map,
                          std::vector<std::shared_ptr<Thing>> &things) {
  float radians = angle * PI / 180.0f;
  sf::Vector2f direction{std::cos(radians), std::sin(radians)};
  sf::Vector2f plane = sf::Vector2f(-direction.y, direction.x) * ASPECT * .5f;

  int xOffset = angle / 90.f * skyTexture.getSize().x;
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

  uint8_t screenPixels[(size_t)SCREEN_W * (size_t)SCREEN_H * 4]{};
  for (size_t y = SCREEN_H / 2; y < SCREEN_H; y++) {
    sf::Vector2f rayDirLeft{direction - plane}, rayDirRight{direction + plane};
    float rowDistance = CAMERA_Z / ((float)y - SCREEN_H / 2);

    sf::Vector2f floorStep =
        rowDistance * (rayDirRight - rayDirLeft) / SCREEN_W;
    sf::Vector2f floor = position + rowDistance * rayDirLeft;

    for (size_t x = 0; x < SCREEN_W; x++) {
      sf::Vector2i cell{floor};

      float textureSize = Resources::texturesImage.getSize().y;
      sf::Vector2i texCoords{textureSize * (floor - (sf::Vector2f)cell)};
      texCoords.x &= (int)textureSize - 1;
      texCoords.y &= (int)textureSize - 1;

      int floorTex = map.getMapCell(floor.x, floor.y, Map::LAYER_FLOOR);
      int ceilTex = map.getMapCell(floor.x, floor.y, Map::LAYER_CEILING);
      sf::Color floorColor, ceilingColor;
      if (floorTex == 0) {
        floorColor = sf::Color(70, 70, 70);
      } else {
        floorColor = Resources::texturesImage.getPixel(
            (floorTex - 1) * textureSize + texCoords.x, texCoords.y);
      }

      if (ceilTex == 0) {
        ceilingColor = sf::Color(0, 0, 0, 0);
      } else {
        ceilingColor = Resources::texturesImage.getPixel(
            (ceilTex - 1) * textureSize + texCoords.x, texCoords.y);
      }

      size_t w = SCREEN_W, h = SCREEN_H;
      size_t floorPixel = (x + y * w) * 4;
      size_t ceilPixel = (x + (h - y - 1) * w) * 4;

      screenPixels[floorPixel + 0] = floorColor.r;
      screenPixels[floorPixel + 1] = floorColor.g;
      screenPixels[floorPixel + 2] = floorColor.b;
      screenPixels[floorPixel + 3] = floorColor.a;

      screenPixels[ceilPixel + 0] = ceilingColor.r;
      screenPixels[ceilPixel + 1] = ceilingColor.g;
      screenPixels[ceilPixel + 2] = ceilingColor.b;
      screenPixels[ceilPixel + 3] = ceilingColor.a;

      floor += floorStep;
    }
  }

  screenBuffer.update(screenPixels);
  target.draw(screenBufferSprite);

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

    int hit{}, isHitVertical{};
    size_t depth = 0;
    while (hit == 0 && depth < MAX_RAYCAST_DEPTH) {
      if (sideDist.x < sideDist.y) {
        sideDist.x += deltaDist.x;
        mapPos.x += step.x;
        isHitVertical = false;
      } else {
        sideDist.y += deltaDist.y;
        mapPos.y += step.y;
        isHitVertical = true;
      }

      hit = map.getMapCell(mapPos.x, mapPos.y, Map::LAYER_WALLS);
      depth++;
    }

    if (hit > 0) {
      float perpWallDist =
          isHitVertical ? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x;
      float wallHeight = SCREEN_H / perpWallDist;

      float wallStart = (-wallHeight + SCREEN_H) / 2.0f;
      float wallEnd = (wallHeight + SCREEN_H) / 2.0f;

      float textureSize = Resources::textures.getSize().y;

      float wallX = isHitVertical ? rayPos.x + perpWallDist * rayDir.x
                                  : rayPos.y + perpWallDist * rayDir.y;
      wallX -= std::floor(wallX);
      int textureX = wallX * textureSize;
      if (!isHitVertical && rayDir.x > 0) textureX = textureSize - textureX - 1;
      if (isHitVertical && rayDir.y < 0) textureX = textureSize - textureX - 1;

      float brightness = 1.0f - (perpWallDist / (float)MAX_RAYCAST_DEPTH);
      if (isHitVertical) { brightness *= 0.7f; }

      sf::Color color =
          sf::Color(255 * brightness, 255 * brightness, 255 * brightness);

      walls.append(
          sf::Vertex(sf::Vector2f(i, wallStart), color,
                     sf::Vector2f(textureX + (hit - 1) * textureSize, 0.0f)));
      walls.append(sf::Vertex(
          sf::Vector2f(i, wallEnd), color,
          sf::Vector2f(textureX + (hit - 1) * textureSize, textureSize)));
      zBuffer[i] = perpWallDist;
    }
  }

  target.draw(walls, {&Resources::textures});

  auto getDistance = [position](const auto &sprite) {
    return std::pow(position.x - sprite->position.x, 2) +
           std::pow(position.y - sprite->position.y, 2);
  };

  auto compare = [getDistance](const auto &a, const auto &b) {
    return getDistance(a) > getDistance(b);
  };

  std::sort(things.begin(), things.end(), compare);

  sf::VertexArray spriteColumns{sf::Lines};
  sf::VertexArray debugColumns{sf::Lines};
  for (const auto &thing : things) {
    if (thing->texture < 0) continue;

    sf::Vector2f spritePos = thing->position - position;

    // Inverse Camera Matrix:
    // det = plane.x*dir.y - plane.y*dir.x
    // [ plane.x dir.x ]-1 = 1/det * [ dir.y     -dir.x  ]
    // [ plane.y dir.y ]             [ -plane.y  plane.x ]
    // Transformed position:
    // 1/det * [ dir.y     -dir.x  ][x] = 1/det * [ dir.y*x    - dir.x*y  ]
    //         [ -plane.y  plane.x ][y]           [ -plane.y*x + plane.x*y]

    float invDet = 1.0f / (plane.x * direction.y - plane.y * direction.x);
    sf::Vector2f transformed{
        invDet * (direction.y * spritePos.x - direction.x * spritePos.y),
        invDet * (-plane.y * spritePos.x + plane.x * spritePos.y),
    };

    int screenX = SCREEN_W / 2 * (1 + transformed.x / transformed.y);
    int spriteSize = std::abs(SCREEN_H / transformed.y);
    int drawStart = -spriteSize / 2 + screenX;
    int drawEnd = spriteSize / 2 + screenX;

    int sizeStart = -spriteSize * thing->size / 2 + screenX;
    int sizeEnd = spriteSize * thing->size / 2 + screenX;

    int start = std::max(drawStart, 0);
    int end = std::min(drawEnd, (int)SCREEN_W - 1);
    for (int i = start; i < end; i++) {
      if (transformed.y > 0.0f && transformed.y < zBuffer[i]) {
        float textureSize = Resources::sprites.getSize().y;
        float texX = thing->texture * textureSize +
                     (i - drawStart) * textureSize / spriteSize;

        sf::Vector2f texStart = {texX, 0}, texEnd = {texX, textureSize};
        sf::Vector2f vertStart(i, -spriteSize / 2.f + SCREEN_H / 2.f);
        sf::Vector2f vertEnd(i, spriteSize / 2.f + SCREEN_H / 2.f);

        spriteColumns.append(sf::Vertex(vertStart, texStart));
        spriteColumns.append(sf::Vertex(vertEnd, texEnd));

        if (i == sizeStart || i == sizeEnd) {
          debugColumns.append(sf::Vertex(vertStart, sf::Color::Green));
          debugColumns.append(sf::Vertex(vertEnd, sf::Color::Green));
        }
      }
    }
  }

  target.draw(spriteColumns, {&Resources::sprites});
  // target.draw(debugColumns);
}
