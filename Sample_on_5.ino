/*
 modified 8/8 to include new stepper position functions. Takes count and compares to postion
 array to retrieve steps
 Used sleep function on easy driver to limit power pull on the stepper, then
 modified 8/16 enable/ disable functions following added capacitor in pump
 
 */
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;         //this line defines the real time clock
//setup pins on arduino
const int dirPin = 11;
const int stepPin = 12;
const int relayPin = 7;                  //pump pin
const int alertPin = 8;              //led to show mactivation, can be removed, just for debugging
//const int sleepPin =  12;
const int enablePin = 10;
//pragram variables
int pumpDuration = 20000;
int count = 0;                    //variable used by runtime loop to keep track of position
int sampleCount = 24;              //number of samples
/*set time interval here Pump and move loop will trigger on. For example if five minutes (12:00, 12:05, 12:10....) is desired
this should be "5", if every 2 (12:00, 12:02 ...) enter "2", for hour enter 60 */
int timeInterval = 5;
const int buttonPin = A1;
int posArray[]= {0, 8, 16, 25, 33, 41, 50, 58, 66, 75, 83, 91, 100, 108, 116, 125, 133, 141, 150, 158,
    166, 175, 183, 192};
void setup()              // this loop runs when first powered on
{
    //setup of pins
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(alertPin, OUTPUT);
    //pinMode(sleepPin, OUTPUT);
    pinMode(enablePin,OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(9600);     //turn on serial monitor for debugging
    digitalWrite(enablePin, HIGH);
    // digitalWrite(sleepPin, LOW);
    Wire.begin();     //wire protocol must be initiated before RTC.
    RTC.begin();
    if(!RTC.isrunning()){
        digitalWrite(alertPin, HIGH);    //if rtc is not connected correctly turn on led solid
    }
    
    
}

void loop()                      //loop runs continuously until power off
{
    DateTime now = RTC.now();      //get current time
    //Serial.print(now.hour(), DEC);
    //Serial.println(now.minute(),DEC);
    Serial.println(now.minute(), DEC);      //print current minute
    int activateCycle = now.minute() % timeInterval;
    if((activateCycle == 0) && (count < sampleCount)) {
        int sampleSteps = posArray[count];       // get step count from positoon array
        int sampleStepsReverse = -1*sampleSteps;  //reverse = negative directions
        digitalWrite(enablePin, LOW);             //activate motor
        rotate(sampleSteps, 0.1);                 //move sample steps at 0.1 turn speed
        delay(2000);                              //catch up
        delay(5000);                             //insert any functions to do while over the sample here
        pumpWater();                            //activate pump relay and pump water
        delay(1000);                            //let drips happen
        rotate(sampleStepsReverse, 0.1);
        delay(10000);
        Serial.println(count);
        count++;
        delay(60000);                            // delay to allow current time to != 5 interval, without this trouble exiting loop
        digitalWrite(enablePin, HIGH);
    }
    //-----end of pump and move loop
    int buttonState = digitalRead(buttonPin);  //check if the reset button is pressed, if so reset count to 0
    if (buttonState == LOW) {
        count = 0;
    }
}               //---------------------------end of runtime loop-------------------

//-----------------------------declare functions--------------------------------//
void pumpWater() {
    digitalWrite(relayPin, HIGH);    //turn pumprelay on,
    delay(pumpDuration);              //delay to allow water to pump
    digitalWrite(relayPin, LOW);       //turn back off
}

void rotate(int steps, float speed){            //this function came from bildr site on easydriver
    int dir = (steps > 0)? HIGH:LOW;
    steps = abs(steps) * 8;                     //8 microsteps per step
    digitalWrite(dirPin, dir);                  
    float usDelay = (1/speed) * 70;             //this controls the speed
    for (int i = 0; i < steps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(usDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(usDelay);
    }
}

