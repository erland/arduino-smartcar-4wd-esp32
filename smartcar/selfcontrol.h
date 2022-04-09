#ifndef SELFCONTROL_H
#define SELFCONTROL_H

#include "movecontroller.h"
#include "collisionmanager.h"

class SelfControl : public MoveController {
  private:
    CollisionManager *collisionManager;
    noDelay refreshTime;
    int leftRotationInProgress;
    int rightRotationInProgress;
    int reverseInProgress;
    int forwardInProgress;
    int dx;
    int dy;
  public:
    SelfControl(CollisionManager *collisionManager, int refreshInterval);
    void refresh();
    virtual int getSpeed();
    virtual int getSteering();
};

#endif
