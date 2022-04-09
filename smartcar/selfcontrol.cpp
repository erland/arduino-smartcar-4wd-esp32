#include "selfcontrol.h"

SelfControl::SelfControl(CollisionManager *collisionManager, int refreshInterval) {
  this->collisionManager = collisionManager;
  this->refreshTime = noDelay(refreshInterval);
  this->dx = 0;
  this->dy = 0;
  this->leftRotationInProgress = 0;
  this->rightRotationInProgress = 0;
  this->forwardInProgress = 0;
  this->reverseInProgress = 0;
}

void SelfControl::refresh() {
  if(this->refreshTime.update()) {
    bool wasForward = false;
    bool wasReverse = false;
    bool wasLeft = false;
    bool wasRight = false;
    if(this->forwardInProgress>0) {
      wasForward = true;
      this->forwardInProgress--;
      if(this->forwardInProgress==0) {
        this->dy = 0;
      }
    }
    if(this->reverseInProgress>0) {
      wasReverse = true;
      this->reverseInProgress--;
      if(this->reverseInProgress==0) {
        this->dy = 0;
      }
    }
    if(this->leftRotationInProgress>0) {
      wasLeft = true;
      this->leftRotationInProgress--;
      if(this->leftRotationInProgress==0) {
        this->dx = 0;
      }
    }
    if(this->rightRotationInProgress>0) {
      wasRight = true;
      this->rightRotationInProgress--;
      if(this->rightRotationInProgress==0) {
        this->dx = 0;
      }
    }

    if(collisionManager->getDistanceToCollision()<40) {
      if(this->forwardInProgress>0) {
        this->forwardInProgress = 0;
        this->dy = 0;
      }else if(this->reverseInProgress>0) {
        // Do nothing
      }else if(this->leftRotationInProgress>0) {
        // Do nothing
      }else if(this->rightRotationInProgress>0) {
        // Do nothing
      }else {
        if(!wasLeft) {
          this->leftRotationInProgress = 100;
          this->dx = -60;
        }else {
          this->rightRotationInProgress = 200;
          this->dx = 60;
        }
      }
    }else {
      if(this->dx != 0 || this->dy != 0) {
        // Do nothing
      }else {
        this->forwardInProgress = 50;
        this->dy = 60;
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
