#include "Arduino.h"
#include <Servo.h>
#include "AutoSolarApi.h"
    /*
     * fadeTime = 25
     * servoWaitTime = 100
     * topServoAngleLimit = 60
     * standbyLedPin = 2
     * topServoPin = 9 (most PWM pins will do)
     * baseServoPin = 10 (again most PWM pins will do)
     */
    /*WARNING: DO NOT MAKE TOPSERVO GO ABOVE 90 DEGREES!*/
    AutoSolarApi panel = AutoSolarApi();
    
    void setup()
    {
        Serial.begin(9600);
        panel.setFadeTimeMilliseconds(25);
        panel.setServoWaitTime(100);
        panel.setTopServoAngleLimit(60);
        panel.setBaseServoAngleLimit(180);
        panel.setServoPowerPin(46);
        panel.setStandbyLedPin(2);
        panel.activateStandbyLedPin(2);
        panel.activateServoPowerPin(46);
        panel.attachTopServoPin(9);
        panel.attachBaseServoPin(10);
    }

    void loop()
    {
        int topAngleLimit = panel.getTopServoAngleLimit();
        float solarBaseArray[panel.getBaseServoAngleLimit()];
        float solarTopArray[topAngleLimit];
        panel.turnServosPowerOn(panel.getServoPowerPin());
        panel.goToTopStartingPosition(panel.getTopServo(), topAngleLimit);
        panel.goToBaseStartingPosition(panel.getBaseServo(), panel.getBaseServoAngleLimit() / 2);
        panel.runBaseScan(panel.getBaseServo(), solarBaseArray);
        panel.goToBestPosition(panel.getBaseServo(), solarBaseArray, panel.getBaseServoAngleLimit());
        panel.runTopScan(panel.getTopServo(), solarTopArray);
        panel.goToBestPosition(panel.getTopServo(), solarTopArray, topAngleLimit);
        panel.standByMode(solarTopArray, topAngleLimit);
        panel.turnServosPowerOff(panel.getServoPowerPin());
        panel.countDown(10);
    }

