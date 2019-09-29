/*
  AutoSolarApi.cpp - Library for controlling motorized solar panels.
  Created by YoungCode, 12/16/16.
*/
/*
 * Class variables are static because they must be a member of AutoSolarApi.
 * Otherwise the variables cannot be declared in the class.
 */
#ifndef AutoSolarApi_h
#define AutoSolarApi_h

#include "Arduino.h"
#include <Servo.h>

class AutoSolarApi
{
  public:
    static int servoWaitTime;
    static int fadeTimeMilliseconds;
    static int topServoAngleLimit;
    static int baseServoAngleLimit;
    static int standbyLedPin;
    static int analogVoltagePin;
    static int servoPowerPin;
    Servo topServo;
    Servo baseServo;
    
    AutoSolarApi();
    
    void countDown(int iteration);
    void standByMode(float theArray[], int arrayLength);
    void runBaseScan(Servo myservo, float baseArray[]);
    void runTopScan(Servo myservo, float topArray[]);
    void goToBaseStartingPosition(Servo myservo, int angle); /*base angle = 90*/
    void goToTopStartingPosition(Servo myservo, int angle);
    void goToXPositionSlowly(Servo myservo, int targetPos);
    void goToBestPosition(Servo myservo, float myArray[], int arrayLength);
    void printArrayValuesToSerialMonitor(float incomingArray[] , int arrayLength);
    void turnServosPowerOn(int pinNumber);
    void turnServosPowerOff(int pinNumber);
    void fadeLed(int pinNumber, int milliSeconds);
    
    int returnCurrentPosition(Servo myservo);
    float returnGreatestArrayValue(float solarArray[], int arrayLength);
    int returnGreatestElement(float solarScanArray[], int arrayLength);
    int returnLastIndexOfFloatSolarScanArray(int arrayLength);
    float returnAnalogPinVoltage(int analogPin);

    int getFadeTimeMilliseconds();
    int getServoWaitTime();
    int getTopServoAngleLimit();
    int getBaseServoAngleLimit();
    int getStandbyLedPin();
    int getServoPowerPin();
    int getSolarBaseArrayLength();
    Servo getTopServo();
    Servo getBaseServo();
    
    void setFadeTimeMilliseconds(int milliseconds);
    void setServoWaitTime(int servoWaitTime);
    void setTopServoAngleLimit(int angle);
    void setBaseServoAngleLimit(int angle);
    void setSolarBaseArrayLength(int baseAngle);
    void setStandbyLedPin(int pinNumber);
    void setServoPowerPin(int pinNumber);
    void attachTopServoPin(int pinNumber);
    void attachBaseServoPin(int pinNumber);
    void activateStandbyLedPin(int pinNumber);
    void activateServoPowerPin(int pinNumber);
    
};

#endif
