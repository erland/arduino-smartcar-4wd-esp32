#ifndef EYECONTROL_H
#define EYECONTROL_H

#include "eye.h"
#include "remotecontrol.h"
#include "collisionmanager.h"

class EyeControl {
  private:
    Eye *leftEye;
    Eye *rightEye;
    noDelay animationTime;
    RemoteControl *remoteControl;
    CollisionManager *collisionManager;
  public:
    EyeControl(RemoteControl *remoteControl, CollisionManager *collisionManager, Eye *leftEye, Eye *rightEye, int refreshInterval);
    void refresh();
};
#endif
