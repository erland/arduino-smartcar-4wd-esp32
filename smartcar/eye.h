#ifndef EYE_H
#define EYE_H

#include <FastLED.h>
#include <NoDelay.h>

#define EYE_NUM_OF_LEDS 7

typedef struct EyeAnimationEntry_struct {
  int delay;
  bool enabled[EYE_NUM_OF_LEDS];
} EyeAnimationEntry;

typedef struct EyeAnimation_struct {
  int length;
  EyeAnimationEntry entries[10];
} EyeAnimation;

extern EyeAnimation EYE_ON;
extern EyeAnimation EYE_OFF;
extern EyeAnimation EYE_BLINK;
extern EyeAnimation EYE_UPPER_RIGHT;
extern EyeAnimation EYE_UPPER_LEFT;




class Eye {
  public:
    CRGB *leds;
    CRGB color;
    CRGB pupilColor;
    EyeAnimation* animation;
    int animationIndex;
    int animationCount;
    noDelay animationTime;
    Eye(CRGB* leds);
    void setAnimation(EyeAnimation& animation, CRGB color, CRGB pupilColor);
    void refresh(bool force = false);
    void forceUpdate();
};

/*
  EyeAnimation EYE_OFF(
  {
    EyeAnimationEntry(
      {false, false, false, false, false, false, false},
      1)
  },
  1
  };

  EyeAnimation EYE_BLINK(
  {
    EyeAnimationEntry(
      {true, true, true, true, true, true, true},
      10),
    EyeAnimationEntry(
      {false, false, false, true, true, true, true},
      10),
    EyeAnimationEntry(
      {false, false, false, true, true, true, false},
      30),
    EyeAnimationEntry(
      {false, false, false, true, true, true, true},
      10),
    EyeAnimationEntry(
      {true, true, true, true, true, true, true},
      100)
  },
  5
  };
*/
#endif
