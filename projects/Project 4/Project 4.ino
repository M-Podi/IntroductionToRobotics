// Define connections to the shift register
const int latchPin = 11;  // Connects to STCP (latch pin) on the shift register
const int clockPin = 10;  // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;   // Connects to DS (data pin) on the shift register
// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;
// Store the digits in an array for easy access
int displayDigits[] = { segD1, segD2, segD3, segD4 };
const int displayCount = 4;  // Number of digits in the display
// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 16;
// Define buzzer pin
const int buzzerPin = 9;

// Define byte encodings for the hexadecimal characters 0-F
byte byteEncodings[encodingsNumber] = {

  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
};

enum StopWatchState {
  RUNNING,
  PAUSED,
  LAP_VIEWING
};

StopWatchState currentState = PAUSED;
// Variables for controlling the display update timing

unsigned long lastIncrement = 0;
unsigned long delayCount = 100;  // Delay between updates (milliseconds)
unsigned long number = 0;        // The number being displayed

const int startStopButton = 2;  // Pin for start/stop button
const int resetButton = 3;      // Pin for reset button
const int saveLapButton = 8;    //Pin for saving a Lap

bool stopwatchRunning = true;
unsigned long lastDebounceTime = 0;  // Last time the output pin was toggled
const long debounceDelay = 100;        // Debounce time in milliseconds

const int joystickXPin = A0;       // Joystick X-axis
const int joystickYPin = A1;       // Joystick Y-axis
const int joystickDeadZone = 200;  // Dead zone for joystick sensitivity
int lastJoystickX = 0;
int lastJoystickY = 0;
unsigned long lastJoystickTime = 0;
const long joystickDebounceDelay = 300;  // Delay for joystick debounce

const int maxLaps = 5;            // Maximum number of laps to store
unsigned long lapTimes[maxLaps];  // Array to store lap times
int currentLapIndex = 0;          // Index for the next lap time to save
int viewLapIndex = 0;             // Index for viewing saved laps

int neutralX, neutralY;


enum Direction { LEFT,
                 RIGHT,
                 NO_MOVEMENT };
Direction getJoystickDirection(int xValue, int yValue);

void setup() {
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  pinMode(startStopButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(saveLapButton, INPUT_PULLUP);

  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);

  neutralX = analogRead(joystickXPin);
  neutralY = analogRead(joystickYPin);

  // Attach interrupts to button pins
  // Enable pin change interrupt for PCINT2 group (pins 0 to 7)
  PCICR |= (1 << PCIE2);
  // Enable interrupts for pins 2 (PCINT18) and 3 (PCINT19)
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);

  // Enable pin change interrupt for PCINT0 group (pins 8 to 13)
  PCICR |= (1 << PCIE0);
  // Enable interrupts for pin 8 (PCINT0)
  PCMSK0 |= (1 << PCINT0);
  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  for (int i = 0; i < maxLaps; i++) {
    lapTimes[i] = 0;
  }
  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}

int findNextNonZeroLapIndex(int currentIndex, bool isForward) {
  int index = currentIndex;
  do {
    // Move to the next or previous index
    index = isForward ? (index + 1) % maxLaps : (index - 1 + maxLaps) % maxLaps;
  } while (lapTimes[index] == 0 && index != currentIndex);  // Stop if we return to the start

  return index;
}

void loop() {
  if (currentState == RUNNING) {
    if (millis() - lastIncrement > delayCount) {
      number++;
      number %= 10000;  // Wrap around after 9999
      lastIncrement = millis();
    }
  } else if (currentState == LAP_VIEWING) {
    int xValue = analogRead(joystickXPin);
    int yValue = analogRead(joystickYPin);

    Direction direction = getJoystickDirection(xValue, yValue);

    if (direction != NO_MOVEMENT) {
      if (millis() - lastJoystickTime > joystickDebounceDelay) {
        bool isForward = (direction == RIGHT);
        viewLapIndex = findNextNonZeroLapIndex(viewLapIndex, isForward);
        number = lapTimes[viewLapIndex];
        lastJoystickTime = millis();
      }
    }
  }
  writeNumber(number);
}


void writeReg(int digit) {
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}
void activateDisplay(int displayNumber) {
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}
void writeNumber(int number) {
  int currentNumber = number;
  for (int displayDigit = 3; displayDigit >= 0; displayDigit--) {
    int lastDigit = currentNumber % 10;
    currentNumber /= 10;

    // Activate the current digit on the display
    activateDisplay(displayDigit);

    // Output the byte encoding for the last digit to the display
    byte var = byteEncodings[lastDigit];
    if (displayDigit == 2) {
      var = var + 1;  // Add the decimal point for the third digit
    }
    writeReg(var);

    // Implement a delay for multiplexing visibility
    delay(0);  // Adjust delay as needed

    // Clear the display to prevent ghosting
    writeReg(B00000000);
  }
}

unsigned long lastDebounceTime1 = 0;  // For button 1
unsigned long lastDebounceTime2 = 0;  // For button 2
unsigned long lastDebounceTime3 = 0;

int lastButtonState1 = HIGH;  // For start/stop button
int lastButtonState2 = HIGH;  // For reset button
int lastButtonState3 = HIGH;


ISR(PCINT2_vect) {
  int readingStartStop = digitalRead(startStopButton);
  int readingReset = digitalRead(resetButton);

  // Handle start/stop button
  if (readingStartStop != lastButtonState1 && (millis() - lastDebounceTime1) > debounceDelay) {
    if (readingStartStop == LOW) {
      tone(buzzerPin, 1000, 100);
      if (currentState == PAUSED) {
        currentState = RUNNING;
      } else if (currentState == RUNNING) {
        currentState = PAUSED;
      }
    }
    lastDebounceTime1 = millis();
  }
  lastButtonState1 = readingStartStop;

  // Reset Button
  if (readingReset != lastButtonState2 && (millis() - lastDebounceTime2) > debounceDelay) {
    if (readingReset == LOW) {
      tone(buzzerPin, 1000, 100);
      if (currentState == PAUSED) {
        lastIncrement = millis();
        number = 0;
        writeNumber(0);
      } else if (currentState == LAP_VIEWING) {
        for (int i = 0; i < maxLaps; i++) {
          lapTimes[i] = 0;
        }
        currentState = PAUSED;
        number = 0;
        writeNumber(0);
        currentLapIndex = 0;
      }
    }
    lastDebounceTime2 = millis();
  }
  lastButtonState2 = readingReset;
}


ISR(PCINT0_vect) {
  int readingSaveLap = digitalRead(saveLapButton);
  if ((readingSaveLap != lastButtonState3) && ((millis() - lastDebounceTime3) > debounceDelay)) {
    if (readingSaveLap == LOW && currentState == RUNNING) {
      tone(buzzerPin, 1000, 100);
      saveLap();
    } else if (readingSaveLap == LOW && currentState == PAUSED) {
      currentState = LAP_VIEWING;
    }
    lastDebounceTime3 = millis();
  }
  lastButtonState3 = readingSaveLap;
}


void saveLap() {
  lapTimes[currentLapIndex] = number;                 // Save the current number in the array
  currentLapIndex = (currentLapIndex + 1) % maxLaps;  // Move to the next index, wrapping around if necessary
}

Direction getJoystickDirection(int xValue, int yValue) {
  if (xValue < neutralX - joystickDeadZone) {
    return LEFT;
  }
  if (xValue > neutralX + joystickDeadZone) {
    return RIGHT;
  }

  return NO_MOVEMENT;
}