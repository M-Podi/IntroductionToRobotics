// Define pin connections for the potentiometers
const int InputRedPin = A0;
const int InputGreenPin = A1;
const int InputBluePin = A2;

// Define the minimum and maximum readings from the potentiometers
const int minInputValue = 15;
const int maxInputValue = 1023;

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
int redInputValue = 0;
int greenInputValue = 0;
int blueInputValue = 0;

// Variables to store current LED brightness levels
int redLedValue = 0;
int greenLedValue = 0;
int blueLedValue = 0;

void setup() {
  // Set potentiometer pins as input
  pinMode(InputRedPin, INPUT);
  pinMode(InputGreenPin, INPUT);
  pinMode(InputBluePin, INPUT);

  // Set LED pins as output
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  // Initialize serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // Read values from potentiometers
  redInputValue = analogRead(InputRedPin);
  greenInputValue = analogRead(InputGreenPin);
  blueInputValue = analogRead(InputBluePin);

  // Map red potentiometer reading to LED brightness
  if(redInputValue < minInputValue) {
    redLedValue = 0;
  } else {
    redLedValue = map(redInputValue, minInputValue, maxInputValue, minLedValue, maxLedValue);
  }

  // Map green potentiometer reading to LED brightness
  if(greenInputValue < minInputValue) {
    greenLedValue = 0;
  } else {
    greenLedValue = map(greenInputValue, minInputValue, maxInputValue, minLedValue, maxLedValue);
  }

  // Map blue potentiometer reading to LED brightness
  if(blueInputValue < minInputValue) {
    blueLedValue = 0;
  } else {
    blueLedValue = map(blueInputValue, minInputValue, maxInputValue, minLedValue, maxLedValue);
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
