/*
stupid-scroll-wheel.ino

Arduino sketch to read the outputs of a brushless motor and convert it to an encoder that 
can be used as a scroll wheel.

http://github.com/rwlloyd/stupid-scroll/wheel

Rob Lloyd. Lincoln. 01/2022.
*/

#include "Keyboard.h"

#define pinA A0
#define pinB A1
#define pinC A2

bool debug = false;  // When true, HID output is disabled and Serial data is output for debugging
                     // See Serial.print() commands in the code. 

// we're not using delay, what with constantly needing to measure the voltage on the analog pins
unsigned long currentMillis;
long previousMillis = 0;
long Interval = 25;   

// Variables to keep track of the BLDC Phases and direction
int AVal = 0;
bool LA = false;
int BVal = 0;
bool LB = false;
int CVal = 0;
bool LC = false;

// Variables for the Direction of movement
int TLevel = 1;                 // Adjust this to get rid of noise on the encDir signal
volatile bool encDir = 0;  

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

void setup() {

  // initialize all the readings to 0:
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  if (debug) {
    Serial.begin(115200);
  }
  // We're not supplying any power to the brushless motor, so the voltage read from the coil pins
  // will struggle to exceed the internal reference voltage unless an someone is really determined
  // to spin the wheel at ridiculous speeds.
  analogReference(INTERNAL);
  // Set the pins to inputs
  pinMode(pinA, INPUT);
  pinMode(pinB, INPUT);
  pinMode(pinC, INPUT);

  // We'll be using the keyboard library
  Keyboard.begin();

}

void loop() {
  readPins();                                         // Reads the motor pins and gets direction
  doSmoothing();                                      // Smooths the output of encoder signals
  currentMillis = millis();
  if (currentMillis - previousMillis >= Interval) {
    if (!debug) {
      if (average > 0) {
        if (encDir) {
          Keyboard.press(KEY_UP_ARROW);
        }
        if (!encDir) {
          Keyboard.press(KEY_DOWN_ARROW);
        }
      }
      Keyboard.releaseAll();
    }
    if (debug) {
      // Serial.print(average);
      // Serial.print(",");
      Serial.println(encDir);
    } 
  }
  delay(25);
}

// Function to get the average of the sum of the phases 
void doSmoothing() {
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = AVal + BVal + CVal;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
}

// Function to read the motor pins analog value and convert it to a digital signal
// yeah, I know, it's a bit hacky. You could refactor the code?! Careful of the encDir though
void readPins() {
  AVal = analogRead(pinA);
  // Convers A to D
  if ((AVal > TLevel) && !LA) {
    LA = true;
    if (LC) {
      // Check the state of the C phase for direction
      encDir = true;
    }
  }
  if ((AVal == 0)) {
    LA = false;
  }
  BVal = analogRead(pinB);
  if ((BVal > TLevel) && !LB) {
    LB = true;
  }
  if ((BVal == 0)) {
    LB = false;
  }

  CVal = analogRead(pinC);
  if ((CVal > TLevel)  && !LC) {
    LC = true;
    if (LA) {
      // Check for the other direction
      encDir = false;
    }
  }
  if ((CVal == 0)) {
    LC = false;
  }

  if (debug) {
    // Print Phase voltages to the Serial port
//    Serial.print(AVal + 2);
//    Serial.print(",");
//    Serial.print(BVal + 4);
//    Serial.print(",");
//    Serial.print(CVal + 6);
//    Serial.print(",");

    // Print phase logic to the serial port
    Serial.print(LA + 2);
    Serial.print(",");
    Serial.print(LB + 4);
    Serial.print(",");
    Serial.print(LC + 6);
    Serial.print(",");
  }
}
