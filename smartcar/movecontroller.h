#ifndef MOVECONTROLLER_H
#define MOVECONTROLLER_H

class MoveController {
  public:
    virtual int getSpeed() = 0;
    virtual int getSteering() = 0;
    virtual bool isForward();
    virtual bool isReverse();
    virtual bool isLeft();
    virtual bool isRight();
    virtual bool isStraight();
    virtual bool isStopped() ;
};
#endif
