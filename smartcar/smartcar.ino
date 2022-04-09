
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
CollisionManager collisionManager(26, 25, 30, 40, 100);
SelfControl selfControl(&collisionManager, 30);
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

  leftEye.setAnimation(EYE_ON, CRGB(255, 255, 0), CRGB(255, 255, 255));
  rightEye.setAnimation(EYE_ON, CRGB(255, 255, 0), CRGB(255, 255, 255));

  remoteControl.init();
  collisionManager.init();
  wheelControl.init();

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(8, 0, angleToPulse(83) );
}



void loop()
{
  collisionManager.refresh();
  remoteControl.refresh();
  smartControl.refresh();
  eyeControl.refresh();
  wheelControl.refresh();

}


int angleToPulse(int ang) {
  int pulse = map(ang, 0, 180, 125, 575); // map angle of 0 to 180 to Servo min and Servo max
  Serial.print("Angle: "); Serial.print(ang);
  Serial.print(" pulse: "); Serial.println(pulse);
  return pulse;
}
