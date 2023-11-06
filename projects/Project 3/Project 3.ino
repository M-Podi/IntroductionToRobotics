
constexpr uint8_t segmentPins[] = { 12, 10, 9, 8, 7, 6, 5, 4 };

constexpr uint8_t joystickPins[] = { A0, A1, 2 };

constexpr uint8_t buzzerPin = 3;

constexpr int buzzerFrequency = 2000;
// Define the dead zone for the joystick to avoid drift when in neutral position
constexpr int deadZone = 200;

constexpr unsigned long blinkInterval = 500;

constexpr unsigned long debounceDelay = 50;


int segmentState = 7;

int pinStates[8] = { 0 };

bool segmentBlinkState = false;
// Store the last time the display was updated
unsigned long previousMillis = 0;
// Store the neutral position values for the joystick
int neutralX, neutralY;

enum Direction { UP,
                 DOWN,
                 LEFT,
                 RIGHT,
                 NO_MOVEMENT };

Direction getJoystickDirection(int xValue, int yValue);

// Transition logic for the 7-segment display depending on the joystick direction
const int moveLogic[8][4] = {
  { -1, 6, 5, 1 }, { 0, 6, 5, -1 }, { 6, 3, 4, 7 }, { 6, -1, 4, 2 }, { 6, 3, -1, 2 }, { 0, 6, -1, 1 }, { 0, 3, -1, -1 }, { -1, -1, 2, -1 }
};


volatile bool buttonPressed = false;
volatile unsigned long buttonPressTime = 0;

// Set up the pins and initialize serial communication
void setup() {
  Serial.begin(9600);
  for (auto pin : segmentPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);  // Start with all LEDs off
  }
  pinMode(joystickPins[2], INPUT_PULLUP);  // Set the button pin with an internal pull-up
  pinMode(buzzerPin, OUTPUT);              // Set the buzzer pin as output
  // Read the neutral joystick position values
  neutralX = analogRead(joystickPins[0]);
  neutralY = analogRead(joystickPins[1]);
  attachInterrupt(digitalPinToInterrupt(joystickPins[2]), handleButtonPress, CHANGE);
}

void handleButtonPress() {
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Debounce the button
  if (interruptTime - lastInterruptTime > debounceDelay) {
    buttonPressed = !buttonPressed;  // Toggle the state
    if (buttonPressed) {
      // Handle the button pressed event
      buttonPressTime = interruptTime;
      tone(buzzerPin, buzzerFrequency);  // Start the buzzer
    } else {
      // Handle the button released event
      noTone(buzzerPin);  // Stop the buzzer
      unsigned long buttonHoldTime = interruptTime - buttonPressTime;
      if (buttonHoldTime >= 3000) {
        // If held for 3 seconds, reset the segment display and play a tone
        for (int i = 0; i < 8; i++) {
          pinStates[i] = 0;
        }
        segmentState = 7;  // Reset to the initial segment state
        Serial.println("Resetting all segments.");
        tone(buzzerPin, buzzerFrequency - 300, 500);  // Play a different tone for reset
      } else {
        // If not held for 3 seconds, just toggle the current segment
        pinStates[segmentState] = !pinStates[segmentState];
        Serial.print("Toggling segment: ");
        Serial.println(segmentState);
      }
    }
  }
  lastInterruptTime = interruptTime;
}

// The main loop where the logic of the program is executed
void loop() {
  // Variables to handle the debounce mechanism
  static unsigned long lastDebounceTime = 0;
  static bool lastButtonState = HIGH;
  static bool buttonState = HIGH;

  // Read the current state of the button
  bool readButtonState = digitalRead(joystickPins[2]);

  // Debounce the button input
  if (readButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  // Check if the button state has stabilized
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Check for a state change
    if (readButtonState != buttonState) {
      buttonState = readButtonState;
      // If the button has been pressed
      if (buttonState == LOW) {
        tone(buzzerPin, buzzerFrequency);  // Start the buzzer
        buttonPressTime = millis();        // Record the time the button was pressed
        buttonPressed = true;
      } else {
        noTone(buzzerPin);  // Stop the buzzer when the button is released
        unsigned long buttonHoldTime = millis() - buttonPressTime;
        // Determine if the button was held for more than 3 seconds
        buttonPressed = false;  // Reset the button press flag
      }
    }
  }

  // Remember the last button state
  lastButtonState = readButtonState;

  // Read the joystick values
  int xValue = analogRead(joystickPins[0]);
  int yValue = analogRead(joystickPins[1]);

  // Handle joystick movement with debounce logic
  if (millis() - lastDebounceTime > debounceDelay) {
    Direction direction = getJoystickDirection(xValue, yValue);
    // Check if the joystick was moved and update the segment state accordingly
    if (direction != NO_MOVEMENT) {
      int nextState = moveLogic[segmentState][direction];
      if (nextState != -1 && nextState != segmentState) {
        segmentState = nextState;
      }
      lastDebounceTime = millis();
    }
  }

  // Update the display with the current segment states
  displaySegmentState();
}

// Function to update the 7-segment display's LED states
void displaySegmentState() {
  unsigned long currentMillis = millis();
  // Handle blinking of the current segment
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    segmentBlinkState = !segmentBlinkState;
  }

  // Update the LEDs of the 7-segment display
  for (size_t i = 0; i < sizeof(segmentPins); i++) {
    if (i == segmentState) {
      digitalWrite(segmentPins[i], segmentBlinkState ? HIGH : LOW);
    } else {
      digitalWrite(segmentPins[i], pinStates[i] ? HIGH : LOW);
    }
  }
}

bool isJoystickInDeadZone = true;

// Function to determine the direction of the joystick movement
Direction getJoystickDirection(int xValue, int yValue) {
  // Check if the joystick is in the dead zone (no movement)
  if (abs(xValue - neutralX) < deadZone && abs(yValue - neutralY) < deadZone) {
    isJoystickInDeadZone = true;
  }

  if (!isJoystickInDeadZone) {
    return NO_MOVEMENT;
  }

  // Determine the direction of movement based on joystick position
  if (xValue < neutralX - deadZone) {
    isJoystickInDeadZone = false;
    return LEFT;
  }
  if (xValue > neutralX + deadZone) {
    isJoystickInDeadZone = false;
    return RIGHT;
  }
  if (yValue < neutralY - deadZone) {
    isJoystickInDeadZone = false;
    return UP;
  }
  if (yValue > neutralY + deadZone) {
    isJoystickInDeadZone = false;
    return DOWN;
  }
  // If none of the conditions are met, there is no movement
  return NO_MOVEMENT;
}