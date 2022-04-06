#include <Arduino.h>
#include "collisionmanager.h"

CollisionManager::CollisionManager(int trigPin, int echoPin) {
  this->collision = false;
  this->nearCollision = false;
  this->collisionRefreshInterval = noDelay(100);
  this->trigPin = trigPin;
  this->echoPin = echoPin;
}

void CollisionManager::init() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void CollisionManager::refresh() {
  if (collisionRefreshInterval.update()) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    long cm = (duration / 2) / 29.1;
    collision = false;
    nearCollision = false;
    if (cm < 25) {
      collision = true;
      nearCollision = true;
    } else if (cm < 35) {
      nearCollision = true;
    }
  }
}

bool CollisionManager::isCollision() {
  return collision;
}

bool CollisionManager::isNearCollision() {
  return nearCollision;
}
