
#include "arduino_secrets.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <FastLED.h>
#include "eye.h"
#include "remotecontrol.h"
#include "collisionmanager.h"
#include "wheelcontrol.h"
#include "eyecontrol.h"
#include "selfcontrol.h"
#include "movecontroller.h"
#include "smartcontrol.h"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
RemoteControl remoteControl(19, 23, 5, 18, 30);
CollisionManager collisionManager(&pwm, 8, 26, 25, 83, 30, 40, 100);
SelfControl selfControl(&collisionManager, 1000, 30);
SmartControl smartControl(&remoteControl, &selfControl);
MotorChannel leftFront(&pwm, 1, 0, 50, 150);
MotorChannel leftBack(&pwm, 3, 2, 60, 160);
MotorChannel rightFront(&pwm, 5, 4, 40, 150);
MotorChannel rightBack(&pwm, 7, 6, 50, 150);
WheelControl wheelControl(&smartControl, &collisionManager, &leftFront, &leftBack, &rightFront, &rightBack, 20);

#define NUM_LEDS 7
CRGB leftEyeLeds[NUM_LEDS];
CRGB rightEyeLeds[NUM_LEDS];
Eye leftEye(leftEyeLeds);
Eye rightEye(rightEyeLeds);
EyeControl eyeControl(&remoteControl, &smartControl, &collisionManager, &leftEye, &rightEye, 20);

void setup()
{
  Serial.begin(115200);

  FastLED.addLeds<WS2812, 27, GRB>(leftEyeLeds, NUM_LEDS);
  FastLED.addLeds<WS2812, 14, GRB>(rightEyeLeds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 50);
  FastLED.clear();
  FastLED.show();
  pwm.begin();
  pwm.setPWMFreq(60);

  leftEye.setAnimation(EYE_ON, CRGB(255, 255, 0), CRGB(255, 255, 255));
  rightEye.setAnimation(EYE_ON, CRGB(255, 255, 0), CRGB(255, 255, 255));

  remoteControl.init();
  collisionManager.init();
  wheelControl.init();

}



void loop()
{
  collisionManager.refresh();
  remoteControl.refresh();
  smartControl.refresh();
  eyeControl.refresh();
  wheelControl.refresh();

}
