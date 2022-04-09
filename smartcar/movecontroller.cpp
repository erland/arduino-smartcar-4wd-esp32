#include "movecontroller.h"

bool MoveController::isForward() {
  return getSpeed() > 0;
}
bool MoveController::isReverse() {
  return getSpeed() < 0;
}
bool MoveController::isLeft() {
  return getSteering() < 0;
}
bool MoveController::isRight() {
  return getSteering() > 0;
}

bool MoveController::isStraight() {
  return getSteering()==0;
}

bool MoveController::isStopped() {
  return getSpeed()==0;
}
