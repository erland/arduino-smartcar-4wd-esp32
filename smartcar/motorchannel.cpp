#include "motorchannel.h"
#include "arduino_secrets.h"

int speedTable[][5] = {
  {0, 0, 0, 0, 0},
  {10, 1, 0, 0, 0},
  {20, 1, 0, 0, 0},
  {30, 1, 0, 0, 0},
  {10, 1, 1, 0, 0},
  {20, 1, 1, 0, 0},
  {30, 1, 1, 0, 0},
  {10, 1, 1, 1, 0},
  {20, 1, 1, 1, 0},
  {30, 1, 1, 1, 0},
  {10, 1, 1, 1, 1}
};

MotorChannel::MotorChannel(Adafruit_PWMServoDriver *pwm, int pinA, int pinB, int minVoltage, int maxVoltage) {
  this->pwm = pwm;
  this->pinA = pinA;
  this->pinB = pinB;
  this->minVoltage = minVoltage;
  this->maxVoltage = maxVoltage;
  moveStep = 0;
  speed = 0;
}

void MotorChannel::setSpeed(int speed) {
  if (this->speed != speed) {
    moveStep = 0;
    this->speed = speed;
  }
}

void MotorChannel::refresh() {
  int speedIndex = (abs(speed) + 9) / 10;
  int adjustedSpeed = minVoltage + (maxVoltage - minVoltage) * abs(speedTable[speedIndex][0]) / 100;
  int multiplier = speedTable[speedIndex][moveStep + 1];
  int voltage = multiplier * adjustedSpeed;

  if (MOTORS_ACTIVE) {
    if (speed > 0) {
      pwm->setPWM(pinA, 0, 4096);
      pwm->setPWM(pinB, 0, voltage * 16);
    } else if (speed < 0) {
      pwm->setPWM(pinA, 0, voltage * 16);
      pwm->setPWM(pinB, 0, 4096);
    } else if (speed == 0) {
      pwm->setPWM(pinA, 0, 4096);
      pwm->setPWM(pinB, 0, 4096);
    }
  }
  moveStep++;
  if (moveStep > 3) {
    moveStep = 0;
  }

}
