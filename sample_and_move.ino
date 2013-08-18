// test to run motor
//  test to run
//
const int dirPin = 11;
const int stepPin = 12;
const int relayPin = 7;
//const int sleepPin = 12;
const int enablePin = 10;

int pumpDuration = 20000;
int count = 0;
int posArray[24]= {
    0, 8, 16, 25, 33, 41, 50, 58, 66, 75, 83, 91, 100, 108, 116, 125, 133, 141, 150, 158,
    166, 175, 183, 192};
int sampleCount = 24;
int buttonPin = A1;

// end of pin setup
void setup(){
    pinMode(stepPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
    pinMode(relayPin, OUTPUT);
    pinMode(buttonPin, INPUT_PULLUP);
    Serial.begin(9600);
    //take out enable when using sleep
    pinMode(enablePin, OUTPUT);
    // digitalWrite(enablePin, HIGH);
    //pinMode(sleepPin, OUTPUT);
    //digitalWrite(sleepPin, LOW);
    digitalWrite(enablePin, LOW);
}
void loop(){
    
    for (count; count<sampleCount; count++){
        int sampleSteps = posArray[count];
        int sampleStepsReverse = -1*sampleSteps;
        // digitalWrite(enablePin, LOW);
        //digitalWrite(sleepPin, HIGH);
        rotate(sampleSteps, 0.1);
        delay(1000);
        //digitalWrite(enablePin, HIGH);
        //digitalWrite(sleepPin, LOW);
        delay(5000);
        
        //insert any functions to do while over the sample here
        pumpWater();
        delay(1000);
        //digitalWrite(enablePin, LOW);
        //digitalWrite(sleepPin, HIGH);
        rotate(sampleStepsReverse, 0.1);
        delay(1000);
        //digitalWrite(enablePin, HIGH);
        //digitalWrite(sleepPin, LOW);
        Serial.println(count);
    }
    
    int buttonState = digitalRead(buttonPin);
    if (buttonState == LOW){
        count = 0;
    }
}

void pumpWater(){
    digitalWrite(relayPin, HIGH);
    delay(pumpDuration);
    digitalWrite(relayPin, LOW);
}
void rotate(int steps, float speed){
    int dir = (steps > 0)? HIGH:LOW;
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