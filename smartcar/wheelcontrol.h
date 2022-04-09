#ifndef WHEELCONTROL_H
#define WHEELCONTROL_H

#include "collisionmanager.h"
#include "movecontroller.h"
#include "motorchannel.h"

class WheelControl {
  private:
    MoveController *moveController;
    CollisionManager *collisionManager;
    int leftSpeed;
    int rightSpeed;
    MotorChannel *leftFront;
    MotorChannel *leftBack;
    MotorChannel *rightFront;
    MotorChannel *rightBack;
    noDelay refreshTime;

    void setRightSpeed(int speed);
    void setLeftSpeed(int speed);
    void setMotorVoltage(int speed, int voltage, int pinA, int pinB);
    int getVoltageForSpeed(int speed, int minVoltage, int maxVoltage);
  public:
    WheelControl(MoveController *moveController, CollisionManager *collisionManager, MotorChannel *leftFront, MotorChannel *leftBack, MotorChannel *rightFront, MotorChannel *rightBack, int refreshInterval);
    void init();
    void refresh();
    int getLeftSpeed();
    int getRightSpeed();
};

#endif
