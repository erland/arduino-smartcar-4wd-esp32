#ifndef EYECONTROL_H
#define EYECONTROL_H

#include "eye.h"
#include "remotecontrol.h"
#include "collisionmanager.h"
#include "movecontroller.h"

class EyeControl {
  private:
    Eye *leftEye;
    Eye *rightEye;
    noDelay animationTime;
    RemoteControl *remoteControl;
    MoveController *moveController;
    CollisionManager *collisionManager;
  public:
    EyeControl(RemoteControl *remoteControl, MoveController *moveController, CollisionManager *collisionManager, Eye *leftEye, Eye *rightEye, int refreshInterval);
    void refresh();
};
#endif
