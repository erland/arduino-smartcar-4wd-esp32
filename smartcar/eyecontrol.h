#ifndef EYECONTROL_H
#define EYECONTROL_H

#include "eye.h"
#include "remotecontrol.h"
#include "collisionmanager.h"

class EyeControl {
  private:
    Eye *leftEye;
    Eye *rightEye;
    RemoteControl *remoteControl;
    CollisionManager *collisionManager;
  public:
    EyeControl(RemoteControl *remoteControl, CollisionManager *collisionManager, Eye *leftEye, Eye *rightEye);
    void refresh();
};
#endif
