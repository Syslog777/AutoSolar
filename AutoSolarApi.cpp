#include "Arduino.h"
#include <Servo.h>
#include "AutoSolarApi.h"

static int AutoSolarApi::fadeTimeMilliseconds;
static int AutoSolarApi::servoWaitTime;
static int AutoSolarApi::topServoAngleLimit;
static int AutoSolarApi::baseServoAngleLimit;
static int AutoSolarApi::standbyLedPin;
static int AutoSolarApi::servoPowerPin;
Servo topServo;
Servo baseServo;

AutoSolarApi::AutoSolarApi()
{
    
}

void AutoSolarApi::countDown(int iteration)
{
    for (int loops = iteration; loops > 0; loops -= 1)
    {
        Serial.println("Time before next system run: ");
        Serial.println(loops - 1);
        delay(iteration * 100);
    }
}

void AutoSolarApi::standByMode(float theArray[], int arrayLength)
{
    float greatestVoltage = returnGreatestArrayValue(theArray, arrayLength);

    turnServosPowerOff(46);
    while (returnAnalogPinVoltage(3) > greatestVoltage / 2)
    {
      Serial.print("The current voltage: ");
      Serial.println(returnAnalogPinVoltage(3));
      delay(300);
      Serial.print("The greatest voltage is: ");
      Serial.println(greatestVoltage);
      delay(300);
      fadeLed(getStandbyLedPin(), fadeTimeMilliseconds);
    }
    Serial.println("DECREASE IN VOLTAGE DETECTED!");
    return;
}

void AutoSolarApi::runBaseScan(Servo myservo, float baseArray[])
{
    int pos;
    delay(1000);

    Serial.println("RUNNING BASE SCAN! " );
    Serial.println("Please wait. :)");
    for (pos = 90; pos <= 180; pos += 1)
    {
      baseArray[pos] = returnAnalogPinVoltage(3);
      myservo.write(pos);
      delay(getServoWaitTime());
    }

    while (returnCurrentPosition(myservo) >= 90)
    {
      pos -= 1;
      myservo.write(pos);
      delay(getServoWaitTime());
    }

    for (pos = 89; pos >= 0; pos -= 1)
    {
      baseArray[pos] = returnAnalogPinVoltage(3);
      myservo.write(pos);
      delay(getServoWaitTime());
    }

    while (returnCurrentPosition(myservo) < 90)
    {
      pos += 1;
      myservo.write(pos);
      delay(getServoWaitTime());
    }
}

void AutoSolarApi::runTopScan(Servo myservo, float topArray[])
{
    int target = getTopServoAngleLimit();
    goToXPositionSlowly(myservo, target);
    delay(1000);
    Serial.println("RUNNING TOP SCAN! ");
    Serial.println("Please wait. :)");
    int pos;
 
    for (pos = target; pos >= 8; pos -= 1)
    { /* changed from pos >= 0 to pos >= 8. Do not go below 8 or you risk damaging the servo*/
      topArray[pos] = returnAnalogPinVoltage(3);
      myservo.write(pos);
      delay(getServoWaitTime());
    }

    for (int initial = 0; initial < 8; initial += 1)
    {
        topArray[initial] = 0;
    }
 
    while (returnCurrentPosition(myservo) < target)
    {
      pos += 1;
      myservo.write(pos);
      delay(getServoWaitTime());
    }
    return;
}

void AutoSolarApi::goToBaseStartingPosition(Servo myservo, int angle)
{
  int base = angle;
  goToXPositionSlowly(myservo, base);   
}

void AutoSolarApi::goToTopStartingPosition(Servo myservo, int angle)
{
    int top = angle;
    goToXPositionSlowly(myservo, top);   
}

void AutoSolarApi::goToXPositionSlowly(Servo myservo, int targetPos)
{
    int startingPos = myservo.read();

    if (startingPos < targetPos)
    {
      for (int pos = startingPos; pos < targetPos; pos += 1)
      {
        myservo.write(pos);
        delay(getServoWaitTime());
      }
    }

    else if (startingPos == targetPos)
    {
      return;
    }

    else if (startingPos > targetPos)
    {
      for (int pos = startingPos; pos > targetPos; pos -= 1)
      {
        myservo.write(pos);
        delay(getServoWaitTime());
      }
      return;
  }    
}

void AutoSolarApi::goToBestPosition(Servo myservo, float myArray[], int arrayLength)
{
    int lastPosition = myservo.read();
    int greatestElement = returnGreatestElement(myArray, arrayLength);
    delay(10);
    int bestPosition = greatestElement;
    int pos;

    Serial.println("GOING TO OPTIMUM POSITION! ");

    if (lastPosition <= bestPosition )
    {
      for (pos = lastPosition; pos <= bestPosition; pos += 1)
      {
        myservo.write(pos);
        delay(getServoWaitTime());
      }
    }
    else if (lastPosition > bestPosition)
    {
      for (pos = lastPosition; pos >= bestPosition; pos -= 1)
      {
        myservo.write(pos);
        delay(getServoWaitTime());
      }
      return;
    }
}

void AutoSolarApi::printArrayValuesToSerialMonitor(float incomingArray[] , int arrayLength)
{
    int index;

    Serial.println("PRINTING ARRAY VALUES!");
    delay(1000);

    for (index = 0; index < arrayLength; index += 1 )
    {
      float theValue = incomingArray[index];
      Serial.println(theValue);
      delay(15);
    }
}

void AutoSolarApi::turnServosPowerOn(int pinNumber)
{
    digitalWrite(pinNumber, HIGH);
}

void AutoSolarApi::turnServosPowerOff(int pinNumber)
{
    digitalWrite(pinNumber, LOW);
}

void AutoSolarApi::fadeLed(int pinNumber, int milliSeconds)
{
    int brightness = 0;
    int fadeAmount = 5;

    while (brightness < 255)
    {
      analogWrite(pinNumber, brightness);
      delay(milliSeconds);
      brightness += fadeAmount;
    }

    while (brightness >= 0)
    {
      analogWrite(pinNumber, brightness);
      delay(milliSeconds);
      brightness -= fadeAmount;
    }

    return;
}

void AutoSolarApi::activateStandbyLedPin(int pinNumber)
{
    pinMode(pinNumber, OUTPUT);
}

float AutoSolarApi::returnGreatestArrayValue(float solarArray[], int arrayLength)
{
    int greatestValue = returnGreatestElement(solarArray, arrayLength);
    float greatestArrayValue =  solarArray[greatestValue];
    return greatestArrayValue;
}

int AutoSolarApi::returnGreatestElement(float solarScanArray[], int arrayLength)
{
    int indexOfGreatestElement;
    int indexOfGreaterElement;
    int index;
    int indexPlusOne = index + 1;
    int lastIndex = returnLastIndexOfFloatSolarScanArray(arrayLength);
    int lengthOfArray = arrayLength;
  
    for (index = 0; index <= lengthOfArray; index += 1)
    {
  
      if (index >= lengthOfArray )
      {
        return indexOfGreatestElement;
      }
  
      else if (max(solarScanArray[index], solarScanArray[indexPlusOne]) == solarScanArray[index])
      {
        indexOfGreaterElement = index;
  
        if (max(solarScanArray[indexOfGreaterElement], solarScanArray[indexOfGreatestElement])
            == solarScanArray[indexOfGreaterElement])
        {
  
          indexOfGreatestElement = indexOfGreaterElement;
          continue;
        }
  
      }
  
      else if (max(solarScanArray[indexPlusOne], solarScanArray[index]) == solarScanArray[indexPlusOne] )
      {
        indexOfGreaterElement = indexPlusOne;
  
        if (max(solarScanArray[indexOfGreaterElement], solarScanArray[indexOfGreatestElement])
            == solarScanArray[indexOfGreaterElement])
        {
  
          indexOfGreatestElement = indexOfGreaterElement;
          continue;
        }
  
      }
  
      else {
        continue;
      }
    }
}

int AutoSolarApi::returnLastIndexOfFloatSolarScanArray(int arrayLength)
{
    int lastIndex = arrayLength - 1 ;
    return lastIndex;
}

float AutoSolarApi::returnAnalogPinVoltage(int analogPin)
{
    int sensorPin = analogRead(analogPin);
    float panelVoltage = sensorPin * (5.0 / 1023.0);
    return panelVoltage;
}

int AutoSolarApi::getFadeTimeMilliseconds()
{
    return fadeTimeMilliseconds;  
}

int AutoSolarApi::getServoWaitTime()
{
    return servoWaitTime;
}

int AutoSolarApi::getTopServoAngleLimit()
{
    return topServoAngleLimit;
}

int AutoSolarApi::getBaseServoAngleLimit()
{
    return baseServoAngleLimit;
}

int AutoSolarApi::getStandbyLedPin()
{
    return standbyLedPin;
}

int AutoSolarApi::getServoPowerPin()
{
    return servoPowerPin;
}

Servo AutoSolarApi::getTopServo()
{
    return topServo;
}

Servo AutoSolarApi::getBaseServo()
{
    return baseServo;
}

void AutoSolarApi::setFadeTimeMilliseconds(int milliseconds)
{
     fadeTimeMilliseconds = milliseconds;
}

void AutoSolarApi::setServoWaitTime(int waitTime)
{
     servoWaitTime = waitTime;  
}

void AutoSolarApi::setTopServoAngleLimit(int angle)
{
    topServoAngleLimit = angle;
}

void AutoSolarApi::setBaseServoAngleLimit(int angle)
{
    baseServoAngleLimit = angle;
}

void AutoSolarApi::setStandbyLedPin(int pinNumber)
{
     standbyLedPin = pinNumber;
}

void AutoSolarApi::setServoPowerPin(int pinNumber)
{
    servoPowerPin = pinNumber;
}

void AutoSolarApi::attachTopServoPin(int pinNumber)
{
    topServo.attach(pinNumber);
}

void AutoSolarApi::attachBaseServoPin(int pinNumber)
{
    baseServo.attach(pinNumber);
}

void AutoSolarApi::activateServoPowerPin(int pinNumber)
{
    pinMode(pinNumber, OUTPUT);
}

int AutoSolarApi::returnCurrentPosition(Servo myservo)
{
    return myservo.read();
}

