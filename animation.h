#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <cstddef>
#include <utility>
#include <vector>

template <typename T> class Animation {
public:
  struct Keyframe {
    float time;
    T value;
  };

  Animation(std::vector<Keyframe> keyframes = {})
      : keyframes(keyframes),
        duration(keyframes.empty() ? 0.f
                                   : keyframes[keyframes.size() - 1].time) {}

  float getDuration() const { return duration; }

  T get(float time) const {
    if (keyframes.empty()) return T();
    if (keyframes.size() == 1) return keyframes[0].value;

    if (time < keyframes[0].time) return keyframes[0].value;
    if (time > keyframes[keyframes.size() - 1].time) {
      return keyframes[keyframes.size() - 1].value;
    }

    size_t i;
    for (size_t j = 0; j < keyframes.size() - 1; j++) {
      if (time < keyframes[j + 1].time) {
        i = j;
        break;
      }
    }

    return keyframes[i].value;
  }

private:
  std::vector<Keyframe> keyframes;
  float duration;
};

enum class FinishAction { None, Loop, Last };

template <typename T> class Animator {
public:
  Animator(T base = T(), std::vector<Animation<T>> animations = {})
      : base(base), animations(animations), current(-1), last(-1), time(),
        action(), lastAction() {}

  void setBase(T newBase) { base = newBase; }

  size_t getAnim() { return current; }

  void setAnim(size_t anim, FinishAction newAction = FinishAction::None) {
    if (anim < animations.size()) {
      last = current;
      current = anim;
      lastAction = action;
      action = newAction;
      time = 0.f;
    }
  }

  void update(float dt) {
    time += dt;
    if (current >= 0 && time >= animations[current].getDuration()) {
      if (action == FinishAction::Loop) {
        time -= animations[current].getDuration();
      } else if (action == FinishAction::Last) {
        time = 0.f;
        current = last;
        action = lastAction;
        last = -1;
      }
    }
  }

  T get() {
    if (current == -1) {
      return base;
    } else {
      return animations[current].get(time);
    }
  }

private:
  T base;
  std::vector<Animation<T>> animations;

  FinishAction action, lastAction;
  size_t current, last;
  float time;
};

#endif // !_ANIMATION_H
