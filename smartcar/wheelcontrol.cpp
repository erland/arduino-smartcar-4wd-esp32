#include "wheelcontrol.h"

WheelControl::WheelControl(MoveController *moveController, CollisionManager *collisionManager, MotorChannel *leftFront, MotorChannel *leftBack, MotorChannel *rightFront, MotorChannel *rightBack, int refreshInterval) {
  this->moveController = moveController;
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
    int rightSpeed = abs(moveController->getSpeed());
    int leftSpeed = rightSpeed;
    if (!moveController->isStopped()) {
      if (moveController->isRight()) {
        rightSpeed = rightSpeed - rightSpeed * abs(moveController->getSteering()) / 100;
        //Serial.print("Forward and steering right: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      } else if (moveController->isLeft()) {
        leftSpeed = leftSpeed - leftSpeed * abs(moveController->getSteering()) / 100;
        //Serial.print("Forward and steering left: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      }

      if (moveController->isForward()) {
        setRightSpeed(rightSpeed);
        setLeftSpeed(leftSpeed);
      } else {
        setRightSpeed(-rightSpeed);
        setLeftSpeed(-leftSpeed);
      }
    } else if (moveController->isLeft() || moveController->isRight()) {
      int steeringSpeed = abs(moveController->getSteering());
      if (moveController->isRight()) {
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
    if (moveController->isStraight() && moveController->isStopped()) {
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
