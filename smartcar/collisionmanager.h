#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include <NoDelay.h>

class CollisionManager {
  private:
    bool collision;
    bool nearCollision;
    int trigPin;
    int echoPin;
    int collisionDistance;
    int nearCollisionDistance;
    noDelay collisionRefreshInterval;
  public:
    CollisionManager(int trigPin, int echoPin, int collisionDistance, int nearCollisionDistance, int refreshInterval);
    void init();
    void refresh();
    bool isCollision();
    bool isNearCollision();
};
#endif
