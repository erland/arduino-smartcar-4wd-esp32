#include "eyecontrol.h"

EyeControl::EyeControl(RemoteControl *remoteControl, MoveController *moveController, CollisionManager *collisionManager, Eye *leftEye, Eye *rightEye, int refreshInterval) {
  this->remoteControl = remoteControl;
  this->collisionManager = collisionManager;
  this->moveController = moveController;
  this->leftEye = leftEye;
  this->rightEye = rightEye;
  this->animationTime = noDelay(refreshInterval);
}

void EyeControl::refresh() {
  if(animationTime.update()) {
    leftEye->refresh(true);
    rightEye->refresh(true);
  }
  if (remoteControl->ps2x.Button(PSB_L1)) {
    leftEye->setAnimation(EYE_UPPER_RIGHT, CRGB(255, 0, 0), CRGB(255, 0, 0));
    rightEye->setAnimation(EYE_UPPER_LEFT, CRGB(255, 0, 0), CRGB(255, 0, 0));
  } else if (remoteControl->ps2x.Button(PSB_R1)) {
    leftEye->setAnimation(EYE_UPPER_LEFT, CRGB(0, 0, 255), CRGB(0, 0, 255));
    rightEye->setAnimation(EYE_UPPER_RIGHT, CRGB(0, 0, 255), CRGB(0, 0, 255));
  } else {
    if (moveController->isRight() && !collisionManager->isCollision()) {
      rightEye->setAnimation(EYE_BLINK, CRGB(255, 215, 0), CRGB(255, 255, 255));
    } else if (moveController->isLeft() && !collisionManager->isCollision()) {
      leftEye->setAnimation(EYE_BLINK, CRGB(255, 215, 0), CRGB(255, 255, 255));
    }
    if (collisionManager->isCollision()) {
      leftEye->setAnimation(EYE_ON, CRGB(255, 0, 0), CRGB(255, 0, 0));
      rightEye->setAnimation(EYE_ON, CRGB(255, 0, 0), CRGB(255, 0, 0));
    } else if (moveController->isForward()) {
      if (!moveController->isLeft()) {
        leftEye->setAnimation(EYE_ON, CRGB(255, 255, 255), CRGB(0, 255, 0));
      }
      if (!moveController->isRight()) {
        rightEye->setAnimation(EYE_ON, CRGB(255, 255, 255), CRGB(0, 255, 0));
      }
    } else if (moveController->isReverse()) {
      if (!moveController->isLeft()) {
        leftEye->setAnimation(EYE_ON, CRGB(255, 0, 0), CRGB(255, 255, 255));
      }
      if (!moveController->isRight()) {
        rightEye->setAnimation(EYE_ON, CRGB(255, 0, 0), CRGB(255, 255, 255));
      }
    } else {
      if (!moveController->isLeft()) {
        leftEye->setAnimation(EYE_ON, CRGB(0, 255, 0), CRGB(0, 255, 0));
      }
      if (!moveController->isRight()) {
        rightEye->setAnimation(EYE_ON, CRGB(0, 255, 0), CRGB(0, 255, 0));
      }
    }
  }
}
