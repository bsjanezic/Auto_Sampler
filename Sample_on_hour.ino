/*
 modified 8/8 to include new stepper position functions. Takes count and compares to postion
 array to retrieve steps
 Used sleep function on easy driver to limit power pull on the stepper, then
 8/10 modified to make array to float instead of int, also changed all dependant values to float.
 8/16 changed pin arragement of dir/step pin and enable pin.
 */
#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 RTC;
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
int sampleCount = 24;      //number of samples
const int buttonPin = A1;
int posArray[24]= {8, 16, 25, 33, 41, 50, 58, 66, 75, 83, 91, 100, 108, 116, 125, 133, 141, 150, 158,
    166, 175, 183, 192, 0};
void setup()              // this loop runs when first powered on
{
    //setup of pins
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(alertPin, OUTPUT);
    // pinMode(sleepPin, OUTPUT);
    pinMode(enablePin,OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(9600);     //turn on serial monitor for debugging
    digitalWrite(enablePin, HIGH);
    //digitalWrite(sleepPin, LOW);
    Wire.begin();     //wire protocol must be initiated before RTC.
    RTC.begin();
    if(!RTC.isrunning()){
        digitalWrite(alertPin, HIGH);    //if rtc is not connected correctly turn on led solid
    }
    
    
}

void loop()                      //loop runs continuously until power off
{
    DateTime now = RTC.now();      //get current time
    Serial.print(now.hour(), DEC);
    //Serial.println(now.minute(),DEC);
    Serial.println(now.minute(), DEC);
    
    if((now.minute()  == 00)
       && (count < sampleCount)){
        int timeHour = now.hour();
        int sampleSteps = posArray[timeHour];       // get step count from positon array
        int sampleStepsReverse = -1*sampleSteps;  //reverse = negative directions
        digitalWrite(enablePin, LOW);
        //digitalWrite(sleepPin, HIGH);           // activate motor then rotate to sample
        rotate(sampleSteps, 0.1);
        delay(2000);                            //catch up
        //digitalWrite(enablePin, HIGH);
        //digitalWrite(sleepPin, LOW);            //sleep motor
        delay(5000);                                                                //insert any functions to do while over the sample here
        pumpWater();                            //activate pump relay and pump water
        delay(1000);                            //let drips happen
        //digitalWrite(enablePin, LOW);
        //digitalWrite(sleepPin, HIGH);           //wake up stepper and return home
        rotate(sampleStepsReverse, 0.1);
        delay(5000);
        
        //digitalWrite(sleepPin, LOW);             //put stepper to sleep
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

void rotate(float steps, float speed){
    float dir = (steps > 0)? HIGH:LOW;
    steps = abs(steps) * 8;
    digitalWrite(dirPin, dir);
    float usDelay = (1/speed) * 70;
    for (int i = 0; i < steps; i++){
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(usDelay);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(usDelay);
    }
}

