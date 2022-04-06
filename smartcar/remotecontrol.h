#ifndef REMOTECONTROL_H
#define REMOTECONTROL_H

#include <PS2X_lib.h>
#include <NoDelay.h>

class RemoteControl {
  private:
    int olddx;
    int olddy;
    int datPin;
    int cmdPin;
    int selPin;
    int clkPin;
    noDelay refreshTime;
  public:
    RemoteControl(int datPin, int cmdPin, int selPin, int clkPin);
    void init();
    void refresh();
    bool isRight();
    bool isLeft();
    bool isForward();
    bool isReverse();
    bool isStopped();
    bool isStraight();
    PS2X ps2x;
    int dy;
    int dx;
    bool changed;
};
#endif
