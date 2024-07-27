#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector2.hpp>

namespace sound {
void play(const sf::SoundBuffer &source);
void play(const sf::SoundBuffer &source, sf::Vector2f position,
          float minDistance = 1.f, bool relative = false);
void update();
} // namespace sound
