#include "renderer.h"
#include "raycast.h"
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

constexpr float PI = 3.141592653589793f;
constexpr float PLAYER_FOV = 60.f;
constexpr size_t MAX_RAYCAST_DEPTH = 64;

Renderer::Renderer() {
  if (!skyTexture.loadFromFile("sky_texture.png")) {
    std::cerr << "Failed to load sky_texture.png!\n";
  }

  skyTexture.setRepeated(true);
}

void Renderer::draw3dView(sf::RenderTarget &target, sf::Vector2f position,
                          float angle, const Map &map,
                          std::vector<std::shared_ptr<Thing>> &things,
                          bool debug) {
  int width = target.getSize().x, height = target.getSize().y;
  float aspect = (float)width / height, cameraZ = height * .5f;

  float radians = angle * PI / 180.0f;
  sf::Vector2f direction{std::cos(radians), std::sin(radians)};
  sf::Vector2f plane = sf::Vector2f(-direction.y, direction.x) * aspect * .5f;

  int xOffset = angle / 90.f * skyTexture.getSize().x;
  while (xOffset < 0) { xOffset += skyTexture.getSize().x; }

  float skyAspect = (float)skyTexture.getSize().x / skyTexture.getSize().y;
  float skyWidth = width;
  float skyHeight = height * aspect / skyAspect;

  sf::Vertex sky[] = {
      sf::Vertex(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(xOffset, 0.0f)),
      sf::Vertex(sf::Vector2f(0.0f, skyHeight),
                 sf::Vector2f(xOffset, skyTexture.getSize().y)),
      sf::Vertex(sf::Vector2f(skyWidth, skyHeight),
                 sf::Vector2f(xOffset + skyTexture.getSize().x,
                              skyTexture.getSize().y)),
      sf::Vertex(sf::Vector2f(skyWidth, 0.0f),
                 sf::Vector2f(xOffset + skyTexture.getSize().x, 0.0f)),
  };

  target.draw(sky, 4, sf::Quads, sf::RenderStates(&skyTexture));

  std::vector<uint8_t> screenPixels(width * height * 4);
  for (size_t y = height / 2; y < height; y++) {
    sf::Vector2f rayDirLeft{direction - plane}, rayDirRight{direction + plane};
    float rowDistance = cameraZ / (y - height / 2.f);

    sf::Vector2f floorStep =
        rowDistance * (rayDirRight - rayDirLeft) / (float)width;
    sf::Vector2f floor = position + rowDistance * rayDirLeft;

    for (size_t x = 0; x < width; x++) {
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

      size_t w = width, h = height;
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

  sf::Texture screenBuffer;
  screenBuffer.create(width, height);
  screenBuffer.update(screenPixels.data());

  sf::Sprite screenBufferSprite{screenBuffer};
  target.draw(screenBufferSprite);

  std::vector<float> zBuffer(width);
  sf::VertexArray walls{sf::Lines};
  for (size_t i = 0; i < width; i++) {
    float cameraX = i * 2.0f / width - 1.0f; // -1.0f -> 0.0f -> 1.0f
    sf::Vector2f rayPos = position;
    sf::Vector2f rayDir = direction + plane * cameraX;
    RayHit hit = raycast(map, rayPos, rayDir);

    if (hit.cell) {
      float wallHeight = height / hit.perpWallDist;

      float wallStart = (-wallHeight + height) / 2.0f;
      float wallEnd = (wallHeight + height) / 2.0f;

      float textureSize = Resources::textures.getSize().y;

      float wallX = hit.isHitVertical ? rayPos.x + hit.perpWallDist * rayDir.x
                                      : rayPos.y + hit.perpWallDist * rayDir.y;
      wallX -= std::floor(wallX);
      int textureX = wallX * textureSize;
      if (!hit.isHitVertical && rayDir.x > 0) {
        textureX = textureSize - textureX - 1;
      }

      if (hit.isHitVertical && rayDir.y < 0) {
        textureX = textureSize - textureX - 1;
      }

      float brightness = 1.0f - (hit.perpWallDist / (float)MAX_RAYCAST_DEPTH);
      if (hit.isHitVertical) { brightness *= 0.7f; }

      sf::Color color(255 * brightness, 255 * brightness, 255 * brightness);

      walls.append(sf::Vertex(
          sf::Vector2f(i, wallStart), color,
          sf::Vector2f(textureX + (hit.cell - 1) * textureSize, 0.0f)));

      walls.append(sf::Vertex(
          sf::Vector2f(i, wallEnd), color,
          sf::Vector2f(textureX + (hit.cell - 1) * textureSize, textureSize)));

      zBuffer[i] = hit.perpWallDist;
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
    int thingTexture =
        thing->animator ? (*thing->animator).get() : thing->texture;

    if (thingTexture < 0) continue;

    sf::Vector2f spritePos = thing->position - position;
    int texture = thingTexture;
    if (thing->directional) {
      sf::Vector2f dir = spritePos;
      float len = std::sqrt(dir.x * dir.x + dir.y * dir.y);
      dir /= len;

      float angle =
          thing->angle - std::atan2(dir.y, dir.x) / M_PI * 180.f + 180.f;
      angle = std::round(angle / 45.f) * 45.f;

      angle = std::fmod(angle, 360.f);
      if (angle < 0.f) angle += 360.f;

      texture += angle / 360.f * 8.f;
    }

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

    int screenX = width / 2.f * (1 + transformed.x / transformed.y);
    int spriteSize = std::abs(height / transformed.y);
    int drawStart = -spriteSize / 2 + screenX;
    int drawEnd = spriteSize / 2 + screenX;

    int sizeStart = -spriteSize * thing->size / 2 + screenX;
    int sizeEnd = spriteSize * thing->size / 2 + screenX;

    int start = std::max(drawStart, 0);
    int end = std::min(drawEnd, (int)width - 1);
    for (int i = start; i < end; i++) {
      if (transformed.y > 0.0f && transformed.y < zBuffer[i]) {
        float textureSize = Resources::sprites.getSize().y;
        float texX = texture * textureSize +
                     (i - drawStart) * textureSize / spriteSize + .01f;

        sf::Vector2f texStart = {texX, 0}, texEnd = {texX, textureSize};
        sf::Vector2f vertStart(i, -spriteSize / 2.f + height / 2.f);
        sf::Vector2f vertEnd(i, spriteSize / 2.f + height / 2.f);

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
  if (debug) { target.draw(debugColumns); }
}
