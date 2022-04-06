#include "eye.h"

#define EYE_PUPIL_LED_NUMBER 0

EyeAnimation EYE_ON {
  1,
  {
    {1, {true, true, true, true, true, true, true}}
  }
};

EyeAnimation EYE_OFF {
  1,
  {
    {1, {false, false, false, false, false, false, false}}
  }
};

EyeAnimation EYE_BLINK {
  5,
  {
    {2, {true, true, true, true, true, true, true}},
    {2, {true, false, false, false, true, true, true}},
    {10, {false, false, false, false, true, true, true}},
    {2, {true, false, false, false, true, true, true}},
    {30, {true, true, true, true, true, true, true}}
  }
};

EyeAnimation EYE_UPPER_RIGHT {
  1,
  {
    {1, {true, true, true, true, false, false, true}}
  }
};

EyeAnimation EYE_UPPER_LEFT {
  1,
  {
    {1, {true, true, true, true, true, false, false}}
  }
};

Eye::Eye(CRGB* leds) {
  this->leds = leds;
  this->color = CRGB(255, 255, 255);
  this->pupilColor = CRGB(255, 255, 255);
  this->animation = NULL;
  this->animationTime = noDelay(20);
}

void Eye::setAnimation(EyeAnimation& animation, CRGB color, CRGB pupilColor) {
  if (&animation != this->animation || color != this->color || pupilColor != this->pupilColor) {
    if (animation.length > 0) {
      this->animation = &animation;
      this->animationIndex = 0;
      this->animationCount = -1;
    } else {
      this->animation = NULL;
    }
    this->color = color;
    this->pupilColor = pupilColor;
    forceUpdate();
  }
}

void Eye::updateAnimation() {
  if (this->animationTime.update() && this->animation != NULL) {
    this->animationCount++;
    if (this->animationCount > this->animation->entries[this->animationIndex].delay) {
      this->animationCount = 0;
      this->animationIndex++;
      if (this->animationIndex >= this->animation->length) {
        this->animationIndex = 0;
      }
    }
    if (this->animationCount == 0) {
      this->forceUpdate();
    }
  }
}
void Eye::forceUpdate() {
  EyeAnimationEntry* entry = &(this->animation->entries[this->animationIndex]);
  for (int i = 0; i < EYE_NUM_OF_LEDS; i++) {
    if (entry->enabled[i]) {
      if (i == EYE_PUPIL_LED_NUMBER) {
        this->leds[i] = this->pupilColor;
      } else {
        this->leds[i] = this->color;
      }
    } else {
      this->leds[i] = CRGB(0, 0, 0);
    }
  }
  FastLED.setBrightness(20);
  FastLED.show();
}
