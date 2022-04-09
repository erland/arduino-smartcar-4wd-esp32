#include "smartcontrol.h"

SmartControl::SmartControl(RemoteControl *remoteControl, SelfControl *selfControl) {
  this->remoteControl = remoteControl;
  this->selfControl = selfControl;
  this->activeControl = remoteControl;
}

void SmartControl::refresh() {
  if (remoteControl->ps2x.Button(PSB_R2)) {
    this->activeControl = this->selfControl;
    this->selfControl->refresh();
  }else {
    this->activeControl = this->remoteControl;
  }
}

int SmartControl::getSpeed() {
  return this->activeControl->getSpeed();
}
int SmartControl::getSteering() {
  return this->activeControl->getSteering();
}
