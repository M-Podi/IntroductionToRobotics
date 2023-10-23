// Define pin connections for the potentiometers
const int PotRedPin = A0;
const int PotGreenPin = A1;
const int PotBluePin = A2;

// Define the minimum and maximum readings from the potentiometers
const int minPotValue = 15;
const int maxPotValue = 1023;

// Define whether the RGB LED is of common anode type (1 for Yes, 0 for No)
const int commonAnode = 0;

// Define pin connections for the RGB LED
const int redLedPin = 11;
const int greenLedPin = 10;
const int blueLedPin = 9;

// Define the minimum and maximum brightness levels for the LED
const int minLedValue = 0;
const int maxLedValue = 255;

// Variables to store current potentiometer readings
int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;

// Variables to store current LED brightness levels
int redLedValue = 0;
int greenLedValue = 0;
int blueLedValue = 0;

void setup() {
  // Set potentiometer pins as input
  pinMode(PotRedPin, INPUT);
  pinMode(PotGreenPin, INPUT);
  pinMode(PotBluePin, INPUT);

  // Set LED pins as output
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read values from potentiometers
  redPotValue = analogRead(PotRedPin);
  greenPotValue = analogRead(PotGreenPin);
  bluePotValue = analogRead(PotBluePin);

  // Map red potentiometer reading to LED brightness
  if(redPotValue < minPotValue) {
    redLedValue = 0;
  } else {
    redLedValue = map(redPotValue, minPotValue, maxPotValue, minLedValue, maxLedValue);
  }

  // Map green potentiometer reading to LED brightness
  if(greenPotValue < minPotValue) {
    greenLedValue = 0;
  } else {
    greenLedValue = map(greenPotValue, minPotValue, maxPotValue, minLedValue, maxLedValue);
  }

  // Map blue potentiometer reading to LED brightness
  if(bluePotValue < minPotValue) {
    blueLedValue = 0;
  } else {
    blueLedValue = map(bluePotValue, minPotValue, maxPotValue, minLedValue, maxLedValue);
  }

  // If using a common anode RGB LED, inverse the LED values
  if (commonAnode) {
    redLedValue = 255 - redLedValue;
    greenLedValue = 255 - greenLedValue;
    blueLedValue = 255 - blueLedValue;
  }

  // Set LED brightness based on mapped values
  analogWrite(redLedPin, redLedValue);
  analogWrite(greenLedPin, greenLedValue);
  analogWrite(blueLedPin, blueLedValue);

  // Print LED brightness values to the serial monitor for debugging
  Serial.print("Red: "); Serial.println(redLedValue);
  Serial.print("Green: "); Serial.println(greenLedValue);
  Serial.print("Blue: "); Serial.println(blueLedValue);
}
