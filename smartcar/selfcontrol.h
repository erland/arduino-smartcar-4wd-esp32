#ifndef SELFCONTROL_H
#define SELFCONTROL_H

#include "movecontroller.h"
#include "collisionmanager.h"

class SelfControl : public MoveController {
  private:
    CollisionManager *collisionManager;
    noDelay refreshTime;
    noDelay sideCollisionTime;
    int leftRotationInProgress;
    int rightRotationInProgress;
    int reverseInProgress;
    int forwardInProgress;
    int dx;
    int dy;
  public:
    SelfControl(CollisionManager *collisionManager, int sideCollisionInterval, int refreshInterval);
    void reset();
    void refresh();
    virtual int getSpeed();
    virtual int getSteering();
};

#endif
