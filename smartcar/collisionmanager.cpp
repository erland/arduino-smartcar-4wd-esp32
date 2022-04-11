#include <Arduino.h>
#include "collisionmanager.h"

CollisionManager::CollisionManager(Adafruit_PWMServoDriver *pwm, int directionPin, int trigPin, int echoPin, int straightAngle, int collisionDistance, int nearCollisionDistance, int refreshInterval) {
  this->collisionDistance = collisionDistance;
  this->nearCollisionDistance = nearCollisionDistance;
  this->collision = false;
  this->nearCollision = false;
  this->collisionRefreshInterval = noDelay(refreshInterval);
  this->trigPin = trigPin;
  this->echoPin = echoPin;
  this->pwm = pwm;
  this->directionPin = directionPin;
  this->straightAngle = straightAngle;
  this->currentDirection = 0;
  this->distanceToCollision = 100; //Arbritary number, will be initialized after first refresh
}

void CollisionManager::init() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pwm->setPWM(this->directionPin, 0, this->angleToPulse(this->straightAngle) );

}

int CollisionManager::checkDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return (duration / 2) / 29.1;
}

void CollisionManager::refresh() {
  if (this->isReady() && abs(this->currentDirection) < 20) {
    if (collisionRefreshInterval.update()) {
      distanceToCollision = checkDistance();
      collision = false;
      nearCollision = false;
      if (distanceToCollision < collisionDistance) {
        collision = true;
        nearCollision = true;
      } else if (distanceToCollision < nearCollisionDistance) {
        nearCollision = true;
      }
    }
  }
}

int CollisionManager::angleToPulse(int angle) {
  int pulse = map(angle, 0, 180, 125, 575); // map angle of 0 to 180 to Servo min and Servo max
  return pulse;
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

void CollisionManager::setCurrentDirection(int angle) {
  if(angle != this->currentDirection) {
    int servoAngle;
    if (this->straightAngle < 90) {
      servoAngle = map(-angle, -90, 90, 0, this->straightAngle * 2);
    } else {
      servoAngle = map(-angle, -90, 90, (this->straightAngle - 90) * 2, 180);
    }
    this->pwm->setPWM(this->directionPin, 0, this->angleToPulse(servoAngle));
    int timeUntilPosition;
    Serial.print("Calculating time between: ");Serial.print(this->currentDirection);Serial.print(" and ");Serial.println(angle);
    if (angle < this->currentDirection) {
      timeUntilPosition = (this->currentDirection - angle) * 150 / 60;
    } else {
      timeUntilPosition = (angle - this->currentDirection) * 150 / 60;
    }
    this->currentDirection = angle;
    Serial.print("Moving to ");Serial.println(angle);
    this->moveUntilTime = millis()+timeUntilPosition;
  }
}

bool CollisionManager::isReady() {
  return millis()>this->moveUntilTime;
}

int CollisionManager::getDistanceAtCurrentDirection() {
  int distance = checkDistance();
  return distance;
}
