#ifndef WHEELCONTROL_H
#define WHEELCONTROL_H

#include "collisionmanager.h"
#include "remotecontrol.h"
#include "motorchannel.h"

class WheelControl {
  private:
    RemoteControl *remoteControl;
    CollisionManager *collisionManager;
    int leftSpeed;
    int rightSpeed;
    MotorChannel *leftFront;
    MotorChannel *leftBack;
    MotorChannel *rightFront;
    MotorChannel *rightBack;
    noDelay remoteCommandTime;
    noDelay carMoveTime;
    int moveStep;

    void refreshMotors();
    void refreshMotorVoltage();
    void setRightSpeed(int speed);
    void setLeftSpeed(int speed);
    void setMotorVoltage(int speed, int voltage, int pinA, int pinB);
    int getVoltageForSpeed(int speed, int minVoltage, int maxVoltage);
  public:
    WheelControl(RemoteControl *remoteControl, CollisionManager *collisionManager, MotorChannel *leftFront, MotorChannel *leftBack, MotorChannel *rightFront, MotorChannel *rightBack);
    void init();
    void refresh();
    int getLeftSpeed();
    int getRightSpeed();
};

#endif
