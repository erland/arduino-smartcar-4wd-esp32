
#include "arduino_secrets.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <FastLED.h>
#include <PS2X_lib.h>
#include "eye.h"

#define PS2_DAT        19  //MISO  19
#define PS2_CMD        23  //MOSI  23
#define PS2_SEL         5  //SS     5
#define PS2_CLK        18  //SLK   18

#define pressures   false
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

int controllerError = -1;
int controllerTryNum = 1;
byte controllerType = 0;


#include <NoDelay.h>

//L9110S motor drive input pin
#define LEFT_1_A  1 //25
#define LEFT_1_B  0 //26
#define LEFT_2_A  3 //16
#define LEFT_2_B  2 //17
#define RIGHT_1_A  5 //14
#define RIGHT_1_B  4 //27
#define RIGHT_2_A  7 // 13
#define RIGHT_2_B  6 //12
#define MOTORS_ACTIVE true

noDelay remoteCommandTime(30);
noDelay collisionCheckTime(100);
noDelay carMoveTime(20);
noDelay remoteControlTime(30);
noDelay blinkersTime(250);

int moveStep = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
#define NUM_LEDS 7
CRGB leftEyeLeds[NUM_LEDS];
CRGB rightEyeLeds[NUM_LEDS];
Eye leftEye(leftEyeLeds);
Eye rightEye(rightEyeLeds);

int carSpeeds[][5] = {
  {0, 0,0,0,0},
  {10, 1,0,0,0},
  {20, 1,0,0,0},
  {30, 1,0,0,0},
  {10, 1,1,0,0},
  {20, 1,1,0,0},
  {30, 1,1,0,0},
  {10, 1,1,1,0},
  {20, 1,1,1,0},
  {30, 1,1,1,0},
  {10, 1,1,1,1}
};

//bool EYE_ALL[NUM_LEDS] = {true, true, true, true, true, true, true};
//bool EYE_OFF[NUM_LEDS] = {false, false, false, false, false, false, false};

#define TRIG_PIN 26
#define ECHO_PIN 25

bool blinkersOn = true;
bool collision = false;
bool nearCollision = false;
void setup()  
{ 
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  FastLED.addLeds<WS2812, 27,GRB>(leftEyeLeds, NUM_LEDS);
  FastLED.addLeds<WS2812, 14,GRB>(rightEyeLeds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 50);
  FastLED.clear();
  FastLED.show();

  leftEye.setAnimation(EYE_ON, CRGB(255,255,0), CRGB(255,255,255));
  rightEye.setAnimation(EYE_ON, CRGB(255,255,0), CRGB(255,255,255));
  
  while (controllerError != 0) {
    delay(3000);// 1 second wait
    //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
    controllerError = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
    Serial.print("#try config ");
    Serial.println(controllerTryNum);
    Serial.print("Error: ");
    Serial.println(controllerError);
    controllerTryNum ++;
  }
  Serial.println(ps2x.Analog(1), HEX);
  
  controllerType = ps2x.readType(); 
  switch(controllerType) {
    case 0:
      Serial.println(" Unknown Controller type found ");
      break;
    case 1:
      Serial.println(" DualShock Controller found ");
      break;
    case 2:
      Serial.println(" GuitarHero Controller found ");
      break;
    case 3:
      Serial.println(" Wireless Sony DualShock Controller found ");
      break;
   }

  pwm.begin();
  pwm.setPWMFreq(60);
  pwm.setPWM(8, 0, angleToPulse(83) );
} 


int dx = 0;
int dy = 0;
int olddx = dx;
int olddy = dy;

void loop()  
{  
    leftEye.updateAnimation();
    rightEye.updateAnimation();
   if(remoteControlTime.update()) {
     ps2x.read_gamepad(false, 0);
     olddy = dy;
     olddx = dx;
      if(ps2x.Analog(PSS_LY) != 128) {
        dy = (128-ps2x.Analog(PSS_LY))*100/128;
      }else if(ps2x.Button(PSB_PAD_UP)) {
        dy = dy + 2;
      }else if(ps2x.Button(PSB_PAD_DOWN)) {
        dy = dy - 2;
      }else {
        dy = 0;
      }
      if(ps2x.Analog(PSS_RX) != 128) {
        dx = -(128-ps2x.Analog(PSS_RX))*100/128;
      }else if(ps2x.Button(PSB_CIRCLE)) {
        dx = dx + 2;
      }else if(ps2x.Button(PSB_SQUARE)) {
        dx = dx - 2;
      }else {
        dx = 0;
      }
      if(dy<-100) {
        dy = -100;
      }
      if(dy>100) {
        dy = 100;
      }
      if(dx<-100) {
        dx = -100;
      }
      if(dx>100) {
        dx = 100;
      }

     if(olddx != dx || olddy != dy) {
      Serial.print("Hastighet: ");
      Serial.print(dy);
      Serial.print(", Styrning: ");
      Serial.println(dx);
     }
   }
   if(ps2x.Button(PSB_L1)) {
      leftEye.setAnimation(EYE_UPPER_RIGHT, CRGB(255,0,0), CRGB(255,0,0));
      rightEye.setAnimation(EYE_UPPER_LEFT, CRGB(255,0,0), CRGB(255,0,0));
   }else if(ps2x.Button(PSB_R1)) {
      leftEye.setAnimation(EYE_UPPER_LEFT, CRGB(0,0,255), CRGB(0,0,255));
      rightEye.setAnimation(EYE_UPPER_RIGHT, CRGB(0,0,255), CRGB(0,0,255));
   }else {
    if(dx>0 && !collision) {
      rightEye.setAnimation(EYE_BLINK, CRGB(255,215,0), CRGB(255,255,255));
    }else if(dx<0 && !collision) {
      leftEye.setAnimation(EYE_BLINK, CRGB(255,215,0), CRGB(255,255,255));
    }
    if(collision) {
      leftEye.setAnimation(EYE_ON, CRGB(255,0,0), CRGB(255,0,0));
      rightEye.setAnimation(EYE_ON, CRGB(255,0,0), CRGB(255,0,0));
    }else if(dy>0) {
      if(dx>=0) {
        leftEye.setAnimation(EYE_ON, CRGB(255,255,255), CRGB(0,255,0));
      }
      if(dx<=0) {
        rightEye.setAnimation(EYE_ON, CRGB(255,255,255), CRGB(0,255,0));
      }
    }else if(dy<0) {
      if(dx>=0) {
        leftEye.setAnimation(EYE_ON, CRGB(255,0,0), CRGB(255,255,255));
      }
      if(dx<=0) {
        rightEye.setAnimation(EYE_ON, CRGB(255,0,0), CRGB(255,255,255));
      }
    }else {
      if(dx>=0) {
        leftEye.setAnimation(EYE_ON, CRGB(0,255,0), CRGB(0,255,0));
      }
      if(dx<=0) {
        rightEye.setAnimation(EYE_ON, CRGB(0,255,0), CRGB(0,255,0));
      }
    }
   }
  
  if(remoteCommandTime.update()) {
    int rightSpeed = abs(dy);
    int leftSpeed = rightSpeed;
    if(rightSpeed>0) {
      if(dx>0) {
        rightSpeed = rightSpeed-rightSpeed*abs(dx)/100;
        //Serial.print("Forward and steering right: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      }else if(dx<0) {
        leftSpeed = leftSpeed-leftSpeed*abs(dx)/100;
        //Serial.print("Forward and steering left: ");
        //Serial.print(leftSpeed);
        //Serial.print(", ");
        //Serial.println(rightSpeed);
      }
      
      if(dy>0) {        
        setRightSpeed(rightSpeed);
        setLeftSpeed(leftSpeed);
      }else {
        setRightSpeed(-rightSpeed);
        setLeftSpeed(-leftSpeed);
      }
    }else if(dx!=0) {
      int steeringSpeed = abs(dx);
      if(dx>0) {   
        //Serial.println("Steering right on place");     
        setRightSpeed(-steeringSpeed);
        setLeftSpeed(steeringSpeed);
      }else {
        //Serial.println("Steering left on place");     
        setRightSpeed(steeringSpeed);
        setLeftSpeed(-steeringSpeed);
      }
    }else {
      setRightSpeed(0);
      setLeftSpeed(0);
    }
  }

   if (dx == 0 && dy == 0) {
      setRightSpeed(0);
      setLeftSpeed(0);
   }
   if(collisionCheckTime.update()) {
     checkCollision();
   }
   if(carMoveTime.update()) {
       if(collision) {
          if(getRightSpeed()>0) {
            setRightSpeed(0);
          }
          if(getLeftSpeed()>0) {
            setLeftSpeed(0);
          }
       }else if(nearCollision) {
          if(getRightSpeed()>30 && getLeftSpeed()>30) {
            setRightSpeed(30);
            setLeftSpeed(30);
          }
       }
      updateCarSpeed();
      moveStep++;
      if(moveStep>3) {
        moveStep = 0;
      }
   }
}

void checkCollision() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  long cm = (duration/2) / 29.1;
  collision = false;
  nearCollision = false;
  if(cm < 25) {
    collision = true;
    nearCollision = true;
  }else if(cm < 35) {
    nearCollision = true;
  }

}

int leftSpeed = 0;
bool changedLeftSpeed = false;
void setLeftSpeed(int speed) {
  if(speed != leftSpeed) {
    changedLeftSpeed = true;
  }
  leftSpeed = speed;
}
int getLeftSpeed() {
  return leftSpeed;
}

int rightSpeed = 0;
bool changedRightSpeed = false;
void setRightSpeed(int speed) {
  if(speed != rightSpeed) {
    changedRightSpeed = true;
  }
  rightSpeed = speed;
}
int getRightSpeed() {
  return rightSpeed;
}

void updateCarSpeed() {
  // Left wheel
  int speedLeftFront = getSpeedVoltage(leftSpeed, 50, 150);
  int speedLeftBack = getSpeedVoltage(leftSpeed, 60, 160);
  if(changedLeftSpeed) {
    changedLeftSpeed = false;
  }
  setMotorVoltage(leftSpeed, speedLeftFront, LEFT_1_A, LEFT_1_B);
  setMotorVoltage(leftSpeed, speedLeftBack, LEFT_2_A, LEFT_2_B);

  // Right
  int speedRightFront = getSpeedVoltage(rightSpeed, 40, 150);
  int speedRightBack = getSpeedVoltage(rightSpeed, 50, 150);
  if(changedRightSpeed) {
    changedRightSpeed = false;
  }

  setMotorVoltage(rightSpeed, speedRightFront, RIGHT_1_A, RIGHT_1_B);
  setMotorVoltage(rightSpeed, speedRightBack, RIGHT_2_A, RIGHT_2_B);
}

void setMotorVoltage(int speed, int voltage, int pinA, int pinB) {

  if(MOTORS_ACTIVE) {
    if(speed>0) {
      pwm.setPWM(pinA,0, 4096);
      pwm.setPWM(pinB, 0, voltage*16);
    }else if(speed<0) {
      pwm.setPWM(pinA, 0, voltage*16);
      pwm.setPWM(pinB, 0, 4096);
    }else if(speed==0) {
      pwm.setPWM(pinA, 0, 4096);
      pwm.setPWM(pinB, 0, 4096);
    }
  }  
}

/*
int setEye(CRGB *eye, bool *shape, CRGB color) {
  for(int i=0;i<NUM_LEDS;i++) {
    if(shape[i]) {
      eye[i] = color;
    }else {
      eye[i] = CRGB(0,0,0);
    }
  }
  FastLED.setBrightness(20);
  FastLED.show();
}
*/
int getSpeedVoltage(int speed, int minSpeed, int maxSpeed) {
  int speedIndex = (abs(speed)+9)/10;
  int adjustedSpeed = minSpeed+(maxSpeed-minSpeed)*abs(carSpeeds[speedIndex][0])/100;
  int multiplier = carSpeeds[speedIndex][moveStep+1];
  return multiplier*adjustedSpeed;
}

int angleToPulse(int ang){
   int pulse = map(ang,0, 180, 125,575);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}
