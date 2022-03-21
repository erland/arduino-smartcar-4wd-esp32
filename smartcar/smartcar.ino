
#include "arduino_secrets.h"

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <PS2X_lib.h>

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


////////////////////////////////////////////// 
//        RemoteXY include library          // 
////////////////////////////////////////////// 

// RemoteXY select connection mode and include library  
#define REMOTEXY_MODE__ESP32CORE_WIFI_CLOUD
#include <WiFi.h> 

#include <RemoteXY.h> 

// RemoteXY connection settings  
#define REMOTEXY_WIFI_SSID SECRET_SSID 
#define REMOTEXY_WIFI_PASSWORD SECRET_PASS 
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com" 
#define REMOTEXY_CLOUD_PORT 6376 
#define REMOTEXY_CLOUD_TOKEN SECRET_CLOUD_TOKEN 

// RemoteXY configurate   
#pragma pack(push, 1) 
uint8_t RemoteXY_CONF[] = 
  { 255,2,0,0,0,19,0,13,13,0,
  4,48,15,7,15,46,2,26,4,176,
  48,23,46,15,2,26 }; 
   
// this structure defines all the variables and events of your control interface  
struct { 

    // input variables
  int8_t slider_1; // =-100..100 slider position 
  int8_t slider_2; // =-100..100 slider position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY; 
#pragma pack(pop) 


///////////////////////////////////////////// 
//           END RemoteXY include          // 
///////////////////////////////////////////// 

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


noDelay remoteCommandTime(30);
noDelay collisionCheckTime(100);
noDelay carMoveTime(20);
noDelay remoteControlTime(30);
int moveStep = 0;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

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


void setup()  
{ 
  Serial.begin(115200);

  pinMode(26, INPUT);
  pinMode(25, INPUT);
  pinMode(17, INPUT);
  pinMode(16, INPUT);
  
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

  RemoteXY_Init ();  

  pwm.begin();
  pwm.setPWMFreq(100);
   
} 

int dx = 0;
int dy = 0;
int olddx = dx;
int olddy = dy;

void loop()  
{  
   RemoteXY_Handler (); 
   if(remoteControlTime.update()) {
     ps2x.read_gamepad(false, 0);
     olddy = dy;
     olddx = dx;
     if(RemoteXY.connect_flag != 0) {
       dy = RemoteXY.slider_1;
       dx = RemoteXY.slider_2;
     }else {
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
     }
     if(olddx != dx || olddy != dy) {
      Serial.print("Hastighet: ");
      Serial.print(dy);
      Serial.print(", Styrning: ");
      Serial.println(dx);
     }
   }
  if(remoteCommandTime.update()) {
    int rightSpeed = abs(dy);
    int leftSpeed = rightSpeed;
    if(rightSpeed>0) {
      if(dx>0) {
        rightSpeed = rightSpeed-rightSpeed*abs(dx)/100;
        Serial.print("Forward and steering right: ");
        Serial.print(leftSpeed);
        Serial.print(", ");
        Serial.println(rightSpeed);
      }else if(dx<0) {
        leftSpeed = leftSpeed-leftSpeed*abs(dx)/100;
        Serial.print("Forward and steering left: ");
        Serial.print(leftSpeed);
        Serial.print(", ");
        Serial.println(rightSpeed);
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
        Serial.println("Stering right on place");     
        setRightSpeed(-steeringSpeed);
        setLeftSpeed(steeringSpeed);
      }else {
        Serial.println("Stering left on place");     
        setRightSpeed(steeringSpeed);
        setLeftSpeed(-steeringSpeed);
      }
    }else {
      setRightSpeed(0);
      setLeftSpeed(0);
    }
  }
  handleCollisions();

   if (RemoteXY.connect_flag == 0 && dx == 0 && dy == 0) {
      setRightSpeed(0);
      setLeftSpeed(0);
   }
   if(carMoveTime.update()) {
      updateCarSpeed();
      moveStep++;
      if(moveStep>3) {
        moveStep = 0;
      }
   }
}

void handleCollisions() {
    int backRight = digitalRead(26);
    int backLeft = digitalRead(25);
    int frontRight = digitalRead(17);
    int frontLeft = digitalRead(16);
    if(backRight == LOW) {
      if(getRightSpeed()<0) {
        Serial.println("Collision back right, stopping car");
        setRightSpeed(0);
      }
      if(getLeftSpeed()<0) {
        Serial.println("Collision back right, stopping car");
        setLeftSpeed(0);
      }
    }
    if(backLeft == LOW) {
      if(getRightSpeed()<0) {
        Serial.println("Collision back left, stopping car");
        setRightSpeed(0);
      }
      if(getLeftSpeed()<0) {
        Serial.println("Collision back left, stopping car");
        setLeftSpeed(0);
      }
    }
    if(frontRight == LOW) {
      if(getRightSpeed()>0) {
        Serial.println("Collision front right, stopping car");
        setRightSpeed(0);
      }
      if(getLeftSpeed()>0) {
        Serial.println("Collision front right, stopping car");
        setLeftSpeed(0);
      }
    }
    if(frontLeft == LOW) {
      if(getRightSpeed()>0) {
        Serial.println("Collision front left, stopping car");
        setRightSpeed(0);
      }
      if(getLeftSpeed()>0) {
        Serial.println("Collision front left, stopping car");
        setLeftSpeed(0);
      }
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

int getSpeedVoltage(int speed, int minSpeed, int maxSpeed) {
  int speedIndex = (abs(speed)+9)/10;
  int adjustedSpeed = minSpeed+(maxSpeed-minSpeed)*abs(carSpeeds[speedIndex][0])/100;
  int multiplier = carSpeeds[speedIndex][moveStep+1];
  return multiplier*adjustedSpeed;
}
