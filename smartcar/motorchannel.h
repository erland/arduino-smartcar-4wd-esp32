#ifndef MOTORCHANNEL_H
#define MOTORCHANNEL_H

#include <Adafruit_PWMServoDriver.h>

class MotorChannel {
  private:
    int speed;
    int moveStep;
    int pinA;
    int pinB;
    int minVoltage;
    int maxVoltage;
    Adafruit_PWMServoDriver *pwm;
  public:
    MotorChannel(Adafruit_PWMServoDriver *pwm, int pinA, int pinB, int minVoltage, int maxVoltage);
    void setSpeed(int speed);
    void refresh();
};

#endif
