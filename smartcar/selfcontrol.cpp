#include "selfcontrol.h"

struct ScanState {
  int angle;
  bool stop;
  int adjustTime;
};

#define NUM_OF_CHECKSTATES 6
ScanState checkStates[NUM_OF_CHECKSTATES] {
  { -80, false, 10},
  { -50, false, 30},
  { -30, true, 60},
  { 30, true, 60},
  {50, false, 30},
  {80, false, 10}
};

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
  this->checkingCollisionIndex = -1;
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
        this->checkingCollisionIndex = -1;
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
              this->leftRotationInProgress = 150;
              this->dx = -60;
            } else {
              Serial.println("Selecting right path");
              this->rightRotationInProgress = 150;
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
  if ((this->checkingCollisionIndex >= 0) && this->collisionManager->isReady()) {
    Serial.print("Checking index "); Serial.println(this->checkingCollisionIndex);
    int distance = this->collisionManager->getDistanceAtCurrentDirection();
    if (distance < 40) {
      if (checkStates[this->checkingCollisionIndex].angle < 0) {
        this->leftRotationInProgress = 0;
        this->rightRotationInProgress = checkStates[this->checkingCollisionIndex].adjustTime;
        this->dx = 60;
        if ( checkStates[this->checkingCollisionIndex].stop) {
          this->dy = 0;
        }
      } else {
        this->leftRotationInProgress = checkStates[this->checkingCollisionIndex].adjustTime;
        this->rightRotationInProgress = 0;
        this->dx = -60;
        if ( checkStates[this->checkingCollisionIndex].stop) {
          this->dy = 0;
        }
      }
      Serial.print("Close at "); Serial.print(checkStates[this->checkingCollisionIndex].angle); Serial.println(", resetting");
      this->checkingCollisionIndex = -1;
      this->collisionManager->setCurrentDirection(0);
    } else {
      this->checkingCollisionIndex++;
      if (this->checkingCollisionIndex < NUM_OF_CHECKSTATES) {
        Serial.print("Not close at "); Serial.print(checkStates[this->checkingCollisionIndex - 1].angle); Serial.println(", moving to next");
        this->collisionManager->setCurrentDirection(checkStates[this->checkingCollisionIndex].angle);
      } else {
        Serial.print("Not close at "); Serial.print(checkStates[this->checkingCollisionIndex - 1].angle); Serial.println(", resetting");
        this->checkingCollisionIndex = -1;
        this->collisionManager->setCurrentDirection(0);
      }
    }
  }
  if (this->collisionManager->isReady()) {
    if (!this->checkingLeftPath && !this->checkingRightPath) {
      if (sideCollisionTime.update()) {
        Serial.println("Initiating checking");
        this->checkingCollisionIndex = 0;
        this->collisionManager->setCurrentDirection(checkStates[this->checkingCollisionIndex].angle);
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
