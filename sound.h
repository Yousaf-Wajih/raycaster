#pragma once

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector3.hpp>

namespace sound {
void play(const sf::SoundBuffer &source);
void play(const sf::SoundBuffer &source, sf::Vector3f position,
          bool relative = false);
void update();
} // namespace sound
