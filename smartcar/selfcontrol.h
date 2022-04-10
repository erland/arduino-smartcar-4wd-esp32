#ifndef SELFCONTROL_H
#define SELFCONTROL_H

#include "movecontroller.h"
#include "collisionmanager.h"

class SelfControl : public MoveController {
  private:
    CollisionManager *collisionManager;
    noDelay refreshTime;
    noDelay sideCollisionTime;
    bool checkingLeft;
    bool checkingRight;
    bool checkingLeftPath;
    bool checkingRightPath;
    int leftDistance;
    int rightDistance;
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
