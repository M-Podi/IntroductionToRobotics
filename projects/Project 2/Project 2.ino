// Define a template-based Queue class
template<typename T>
class Queue {
  // Nested node structure for a singly linked list representation of the queue
private:
  struct Node {
    T data;
    Node* next;

    Node(T value)
      : data(value), next(nullptr) {}
  };

  Node* front;  // pointer to the front node
  Node* rear;   // pointer to the rear node
  int size;     // size of the queue

public:
  // Default constructor
  Queue()
    : front(nullptr), rear(nullptr), size(0) {}

  // Destructor to deallocate the memory used by the queue
  ~Queue() {
    while (!isEmpty()) {
      dequeue();
    }
  }

  // Function to add an element to the end of the queue
  void enqueue(T value) {
    Node* newNode = new Node(value);
    if (!rear) {
      front = rear = newNode;
    } else {
      rear->next = newNode;
      rear = newNode;
    }
    size++;
  }

  // Function to remove and return an element from the front of the queue
  T dequeue() {
    if (isEmpty()) {
      return T();
    }
    T result = front->data;
    Node* temp = front;
    front = front->next;
    if (!front) {
      rear = nullptr;
    }
    delete temp;
    size--;
    return result;
  }

  // Function to return the front element without removing it
  T getFront() const {
    if (isEmpty()) {
      return T();
    }
    return front->data;
  }

  // Check if the queue is empty
  bool isEmpty() const {
    return !front;
  }

  // Get the current size of the queue
  int getSize() const {
    return size;
  }
};

// Constants for different pins
const int floorLEDs[] = { 2, 3, 4 };
const int operationLED = 5;
const int buttonPins[] = { 6, 7, 8 };
const int buzzer = 9;

// Elevator state variables
int currentFloor = 0;
bool isMoving = false;
Queue<int> floorQueue;
volatile bool toggleLED = false;
bool operationLEDState = LOW;

// Debouncing variables for button press handling
unsigned long lastDebounceTime[3] = {0, 0, 0}; 
const long debounceDelay = 50;

// Timing variables for elevator motion and blinking
unsigned long previousMillis = 0;
unsigned long elevatorMoveMillis = 0;
unsigned long blinkDelay = 300;
const long floorDelay = 2000;
bool startMoveDelayOver = false;
unsigned long startMoveDelayStart = 0;
const long startMoveDelayDuration = 2000;
unsigned long beepDuration = 100;
unsigned long beepFreq = 2000;
unsigned long movFreq = 1000;

// ISR functions to handle button presses
void handleButtonPress(int buttonIndex);

void buttonISR_1() { handleButtonPress(0); }
void buttonISR_2() { handleButtonPress(1); }
void buttonISR_3() { handleButtonPress(2); }

void (*buttonISR[3])() = {buttonISR_1, buttonISR_2, buttonISR_3};

// Setup function for Arduino initialization
void setup() {
  Serial.begin(9600);

  // Set LED pins as output
  for (int i = 0; i < 3; i++) {
    pinMode(floorLEDs[i], OUTPUT);
  }
  pinMode(operationLED, OUTPUT);
  digitalWrite(floorLEDs[0], HIGH);
  digitalWrite(operationLED, HIGH);

  // Set button pins as input and attach interrupts
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), buttonISR[i], RISING);
  }
}

bool pendingMove = false;
int targetFloor = -1;

// Main loop
void loop() {
  // Check each button for presses, and enqueue if necessary
  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading == LOW) { 
      if ((millis() - lastDebounceTime[i]) > debounceDelay) {
        if (i != currentFloor) {
          floorQueue.enqueue(i);
          Serial.println("Button for floor " + String(i + 1) + " pressed.");
        }
      }
      lastDebounceTime[i] = millis();
    }
  }

  // Handle the case when the elevator is not currently moving to a target
  if (!pendingMove && !floorQueue.isEmpty()) {
    targetFloor = floorQueue.dequeue();
    isMoving = true;
    pendingMove = true;
    startMoveDelayOver = false;
    startMoveDelayStart = millis();
    tone(buzzer, 1000,100);
  }

  if (pendingMove) {
    moveElevatorStep(targetFloor);
  }
}

// Function to enqueue the floor request on button press
void handleButtonPress(int buttonIndex) {
  if ((millis() - lastDebounceTime[buttonIndex]) > debounceDelay) {
    floorQueue.enqueue(buttonIndex + 1);
    toggleLED = true;
  }
  lastDebounceTime[buttonIndex] = millis();
}

unsigned long floorPauseStart = 0;
bool inFloorPause = false;

// Function to move the elevator one step at a time
void moveElevatorStep(int targetFloor) {
  unsigned long currentMillis = millis();

  // Blink the operation LED
  if (currentMillis - previousMillis >= blinkDelay) {
    previousMillis = currentMillis;
    operationLEDState = !operationLEDState;
    digitalWrite(operationLED, operationLEDState);
  }

  // Wait before starting the elevator movement
  if (!startMoveDelayOver && (currentMillis - startMoveDelayStart) < startMoveDelayDuration) {        
    return;
  } else {
    startMoveDelayOver = true;
  }

  tone(buzzer,moveFreq);
  int floorDirection = targetFloor > currentFloor ? 1 : -1;

  // Pause at the floor before proceeding
  if (inFloorPause) {
    if (currentMillis - floorPauseStart >= floorDelay) {
      inFloorPause = false;
    }
    return;
  }

  // Move the elevator one floor after a delay
  if (currentMillis - elevatorMoveMillis >= floorDelay*2) {
    digitalWrite(floorLEDs[currentFloor], LOW);
    currentFloor += floorDirection;
    digitalWrite(floorLEDs[currentFloor], HIGH);
    elevatorMoveMillis = currentMillis;
  }

  noTone(buzzer);

  // Handle the elevator reaching the target floor
  if (currentFloor == targetFloor && !inFloorPause) {
    pendingMove = false;
    isMoving = false;
    tone(buzzer, beepFreq, beepDuration);
    operationLEDState = HIGH;
    digitalWrite(operationLED, operationLEDState);
    inFloorPause = true;
    floorPauseStart = currentMillis;
  }
}
