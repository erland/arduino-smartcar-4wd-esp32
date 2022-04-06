#include "remotecontrol.h"

#define pressures   false
#define rumble      false

RemoteControl::RemoteControl(int datPin, int cmdPin, int selPin, int clkPin) {
  this->datPin = datPin;
  this->cmdPin = cmdPin;
  this->selPin = selPin;
  this->clkPin = clkPin;
  this->refreshTime = noDelay(30);
  this->dx = 0;
  this->dy = 0;
  this->olddx = dx;
  this->olddy = dy;
}

void RemoteControl::init() {
  int controllerError = -1;
  int controllerTryNum = 1;
  while (controllerError != 0) {
    delay(3000);// 1 second wait
    //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    controllerError = ps2x.config_gamepad(this->clkPin, this->cmdPin, this->selPin, this->datPin, pressures, rumble);
    Serial.print("#try config ");
    Serial.println(controllerTryNum++);
    Serial.print("Error: ");
    Serial.println(controllerError);
  }
  Serial.println(ps2x.Analog(1), HEX);

  int controllerType = ps2x.readType();
  switch (controllerType) {
    case 0:
      Serial.println(" Unknown Controller type found ");
      break;
    case 1:
      Serial.println(" DualShock Controller found ");
      break;
    case 2:
      Serial.println(" GuitarHero Controller found ");
      break;
    case 3:
      Serial.println(" Wireless Sony DualShock Controller found ");
      break;
  }
}

void RemoteControl::refresh() {
  if (refreshTime.update()) {
    ps2x.read_gamepad(false, 0);
    olddy = dy;
    olddx = dx;
    if (ps2x.Analog(PSS_LY) != 128) {
      dy = (128 - ps2x.Analog(PSS_LY)) * 100 / 128;
    } else if (ps2x.Button(PSB_PAD_UP)) {
      dy = dy + 2;
    } else if (ps2x.Button(PSB_PAD_DOWN)) {
      dy = dy - 2;
    } else {
      dy = 0;
    }
    if (ps2x.Analog(PSS_RX) != 128) {
      dx = -(128 - ps2x.Analog(PSS_RX)) * 100 / 128;
    } else if (ps2x.Button(PSB_CIRCLE)) {
      dx = dx + 2;
    } else if (ps2x.Button(PSB_SQUARE)) {
      dx = dx - 2;
    } else {
      dx = 0;
    }
    if (dy < -100) {
      dy = -100;
    }
    if (dy > 100) {
      dy = 100;
    }
    if (dx < -100) {
      dx = -100;
    }
    if (dx > 100) {
      dx = 100;
    }

    if (olddx != dx || olddy != dy) {
      Serial.print("Hastighet: ");
      Serial.print(dy);
      Serial.print(", Styrning: ");
      Serial.println(dx);
    }
  }
}

bool RemoteControl::isForward() {
  return dy > 0;
}
bool RemoteControl::isReverse() {
  return dy < 0;
}
bool RemoteControl::isLeft() {
  return dx < 0;
}
bool RemoteControl::isRight() {
  return dx > 0;
}
bool RemoteControl::isStopped() {
  return dy == 0;
}
bool RemoteControl::isStraight() {
  return dx == 0;
}
