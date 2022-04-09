#include <Arduino.h>
#include "collisionmanager.h"

CollisionManager::CollisionManager(int trigPin, int echoPin, int collisionDistance, int nearCollisionDistance, int refreshInterval) {
  this->collisionDistance = collisionDistance;
  this->nearCollisionDistance = nearCollisionDistance;
  this->collision = false;
  this->nearCollision = false;
  this->collisionRefreshInterval = noDelay(refreshInterval);
  this->trigPin = trigPin;
  this->echoPin = echoPin;
  this->distanceToCollision = 100; //Arbritary number, will be initialized after first refresh
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
    distanceToCollision = cm;
    collision = false;
    nearCollision = false;
    if (cm < collisionDistance) {
      collision = true;
      nearCollision = true;
    } else if (cm < nearCollisionDistance) {
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
int CollisionManager::getDistanceToCollision() {
  return distanceToCollision;
}
