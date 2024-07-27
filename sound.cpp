#include "sound.h"

#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <list>
#include <memory>

namespace sound {

namespace {
std::list<std::shared_ptr<sf::Sound>> sounds;
}

void play(const sf::SoundBuffer &source) { play(source, {}, 1.f, true); }

void play(const sf::SoundBuffer &source, sf::Vector2f position,
          float minDistance, bool relative) {
  std::shared_ptr sound = std::make_shared<sf::Sound>(source);
  sounds.push_back(sound);

  sound->play();
  sound->setPosition({position.x, position.y, 0.f});
  sound->setRelativeToListener(relative);
  sound->setMinDistance(minDistance);
}

void update() {
  std::list<decltype(sounds)::iterator> toDelete;
  for (auto it = sounds.begin(); it != sounds.end(); it++) {
    auto sound = *it;
    if (sound->getStatus() == sf::Sound::Stopped) { toDelete.push_back(it); }
  }

  for (const auto &it : toDelete) { sounds.erase(it); }
}

} // namespace sound
