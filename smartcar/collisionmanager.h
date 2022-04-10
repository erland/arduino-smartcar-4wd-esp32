#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <Adafruit_PWMServoDriver.h>
#include <NoDelay.h>

class CollisionManager {
  private:
    bool collision;
    bool nearCollision;
    int trigPin;
    int echoPin;
    int collisionDistance;
    int nearCollisionDistance;
    int distanceToCollision;
    noDelay collisionRefreshInterval;
    int directionPin;
    int straightAngle;
    Adafruit_PWMServoDriver *pwm;
    int currentDirection;
    long moveUntilTime;
    int angleToPulse(int angle);
    int checkDistance();
  public:
    CollisionManager(Adafruit_PWMServoDriver *pwm, int directionPin, int trigPin, int echoPin, int straightAngle, int collisionDistance, int nearCollisionDistance, int refreshInterval);
    void init();
    void refresh();
    void setCurrentDirection(int direction);
    int getDistanceToCollision();
    int getDistanceAtCurrentDirection();
    bool isReady();
    bool isCollision();
    bool isNearCollision();
};
#endif
