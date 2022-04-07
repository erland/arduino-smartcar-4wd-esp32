#include "wheelcontrol.h"

WheelControl::WheelControl(RemoteControl *remoteControl, CollisionManager *collisionManager, MotorChannel *leftFront, MotorChannel *leftBack, MotorChannel *rightFront, MotorChannel *rightBack, int refreshInterval) {
  this->remoteControl = remoteControl;
  this->collisionManager = collisionManager;
  this->leftFront = leftFront;
  this->leftBack = leftBack;
  this->rightFront = rightFront;
  this->rightBack = rightBack;
  this->leftSpeed = 0;
  this->rightSpeed = 0;
  this->refreshTime = noDelay(refreshInterval);
}

void WheelControl::init() {
  // Do nothing right now
}

void WheelControl::refresh() {
  if (refreshTime.update()) {
    int rightSpeed = abs(remoteControl->dy);
    int leftSpeed = rightSpeed;
    if (!remoteControl->isStopped()) {
      if (remoteControl->isRight()) {
        rightSpeed = rightSpeed - rightSpeed * abs(remoteControl->dx) / 100;
        //Serial.print("Forward and steering right: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      } else if (remoteControl->isLeft()) {
        leftSpeed = leftSpeed - leftSpeed * abs(remoteControl->dx) / 100;
        //Serial.print("Forward and steering left: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      }

      if (remoteControl->isForward()) {
        setRightSpeed(rightSpeed);
        setLeftSpeed(leftSpeed);
      } else {
        setRightSpeed(-rightSpeed);
        setLeftSpeed(-leftSpeed);
      }
    } else if (remoteControl->isLeft() || remoteControl->isRight()) {
      int steeringSpeed = abs(remoteControl->dx);
      if (remoteControl->isRight()) {
        //Serial.println("Steering right on place");
        setRightSpeed(-steeringSpeed);
        setLeftSpeed(steeringSpeed);
      } else {
        //Serial.println("Steering left on place");
        setRightSpeed(steeringSpeed);
        setLeftSpeed(-steeringSpeed);
      }
    } else {
      setRightSpeed(0);
      setLeftSpeed(0);
    }
    if (remoteControl->isStraight() && remoteControl->isStopped()) {
      setRightSpeed(0);
      setLeftSpeed(0);
    }
    if (collisionManager->isCollision()) {
      if (getRightSpeed() > 0) {
        setRightSpeed(0);
      }
      if (getLeftSpeed() > 0) {
        setLeftSpeed(0);
      }
    } else if (collisionManager->isNearCollision()) {
      if (getRightSpeed() > 30 && getLeftSpeed() > 30) {
        setRightSpeed(30);
        setLeftSpeed(30);
      }
    }
    leftFront->refresh();
    leftBack->refresh();
    rightFront->refresh();
    rightBack->refresh();
  }
}


void WheelControl::setLeftSpeed(int speed) {
  leftSpeed = speed;
  leftFront->setSpeed(speed);
  leftBack->setSpeed(speed);
}
int WheelControl::getLeftSpeed() {
  return leftSpeed;
}

void WheelControl::setRightSpeed(int speed) {
  rightSpeed = speed;
  rightFront->setSpeed(speed);
  rightBack->setSpeed(speed);
}
int WheelControl::getRightSpeed() {
  return rightSpeed;
}
