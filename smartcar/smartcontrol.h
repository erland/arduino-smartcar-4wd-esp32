#ifndef SMARTCONTROL_H
#define SMARTCONTROL_H

#include "movecontroller.h"
#include "remotecontrol.h"
#include "selfcontrol.h"

class SmartControl : public MoveController {
  private:
    RemoteControl *remoteControl;
    SelfControl *selfControl;
    MoveController *activeControl;
  public:
    SmartControl(RemoteControl *remoteControl, SelfControl *selfControl);
    void refresh();
    virtual int getSpeed();
    virtual int getSteering();
};
#endif
