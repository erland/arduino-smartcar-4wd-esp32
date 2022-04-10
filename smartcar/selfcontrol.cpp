#include "selfcontrol.h"

SelfControl::SelfControl(CollisionManager *collisionManager, int sideCollisionInterval, int refreshInterval) {
  this->collisionManager = collisionManager;
  this->refreshTime = noDelay(refreshInterval);
  this->sideCollisionTime = noDelay(sideCollisionInterval);
  this->reset();
}

void SelfControl::reset() {
  this->dx = 0;
  this->dy = 0;
  this->leftRotationInProgress = 0;
  this->rightRotationInProgress = 0;
  this->forwardInProgress = 0;
  this->reverseInProgress = 0;
  this->checkingLeft = false;
  this->checkingRight = false;
  this->checkingLeftPath = false;
  this->checkingRightPath = false;
  this->leftDistance = 0;
  this->rightDistance = 0;
  this->collisionManager->setCurrentDirection(0);
}

void SelfControl::refresh() {
  bool searchingDirection = false;
  if (this->refreshTime.update()) {
    bool wasForward = false;
    bool wasReverse = false;
    bool wasLeft = false;
    bool wasRight = false;
    if (this->forwardInProgress > 0) {
      wasForward = true;
      this->forwardInProgress--;
      if (this->forwardInProgress == 0) {
        this->dy = 0;
      }
    }
    if (this->reverseInProgress > 0) {
      wasReverse = true;
      this->reverseInProgress--;
      if (this->reverseInProgress == 0) {
        this->dy = 0;
      }
    }
    if (this->leftRotationInProgress > 0) {
      wasLeft = true;
      this->leftRotationInProgress--;
      if (this->leftRotationInProgress == 0) {
        this->dx = 0;
      }
    }
    if (this->rightRotationInProgress > 0) {
      wasRight = true;
      this->rightRotationInProgress--;
      if (this->rightRotationInProgress == 0) {
        this->dx = 0;
      }
    }

    if (collisionManager->getDistanceToCollision() < 40) {
      if (this->forwardInProgress > 0) {
        this->forwardInProgress = 0;
        this->dy = 0;
      } else if (this->reverseInProgress > 0) {
        // Do nothing
      } else if (this->leftRotationInProgress > 0) {
        // Do nothing
      } else if (this->rightRotationInProgress > 0) {
        // Do nothing
      } else {
        this->checkingLeft = false;
        this->checkingRight = false;
        if (this->checkingLeftPath) {
          if (this->collisionManager->isReady()) {
            this->checkingLeftPath = false;
            this->leftDistance = this->collisionManager->getDistanceAtCurrentDirection();
            Serial.println("Initiating checking right path");
            this->collisionManager->setCurrentDirection(90);
            this->checkingRightPath = true;
          }
        } else if (this->checkingRightPath) {
          if (this->collisionManager->isReady()) {
            this->checkingRightPath = false;
            this->rightDistance = this->collisionManager->getDistanceAtCurrentDirection();
            Serial.println("Resetting");
            this->collisionManager->setCurrentDirection(0);
            if (this->leftDistance > this->rightDistance) {
              Serial.println("Selecting left path");
              this->leftRotationInProgress = 80;
              this->dx = -60;
            } else {
              Serial.println("Selecting right path");
              this->rightRotationInProgress = 80;
              this->dx = 60;
            }
          }
        } else {
          if (this->collisionManager->isReady()) {
            Serial.println("Initiating checking left path");
            this->collisionManager->setCurrentDirection(-90);
            this->checkingLeftPath = true;
          }
        }
      }
    } else {
      if (this->dx != 0 || this->dy != 0) {
        // Do nothing
      } else {
        this->forwardInProgress = 50;
        this->dy = 60;
      }
    }
  }
  if ((this->checkingLeft || this->checkingRight) && this->collisionManager->isReady()) {
    int distance = this->collisionManager->getDistanceAtCurrentDirection();
    if (this->checkingLeft) {
      this->checkingLeft = false;
      if (distance < 40) {
        Serial.println("Left close, resetting");
        this->leftRotationInProgress = 0;
        this->rightRotationInProgress = 25;
        this->dx = 60;
        this->collisionManager->setCurrentDirection(0);
      } else {
        Serial.println("Left not close, initiating right check");
        this->collisionManager->setCurrentDirection(70);
        this->checkingRight = true;
      }
    } else {
      this->checkingRight = false;
      if (distance < 40) {
        Serial.println("Right close, resetting");
        this->leftRotationInProgress = 25;
        this->rightRotationInProgress = 0;
        this->dx = -60;
      } else {
        Serial.println("Right not close, resetting");
      }
      this->collisionManager->setCurrentDirection(0);
    }
  }
  if (this->collisionManager->isReady()) {
    if (!this->checkingLeftPath && !this->checkingRightPath) {
      if (sideCollisionTime.update()) {
        Serial.println("Initiating checking left");
        this->collisionManager->setCurrentDirection(-70);
        this->checkingLeft = true;
      }
    }
  }
}

int SelfControl::getSpeed() {
  return this->dy;
}

int SelfControl::getSteering() {
  return this->dx;
}
