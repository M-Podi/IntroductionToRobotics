template<typename T>
class Queue {
private:
  struct Node {
    T data;
    Node* next;

    Node(T value)
      : data(value), next(nullptr) {}
  };

  Node* front;
  Node* rear;
  int size;

public:
  Queue()
    : front(nullptr), rear(nullptr), size(0) {}

  ~Queue() {
    while (!isEmpty()) {
      dequeue();
    }
  }

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

  T getFront() const {
    if (isEmpty()) {
      return T();
    }

    return front->data;
  }

  bool isEmpty() const {
    return !front;
  }

  int getSize() const {
    return size;
  }
  bool contains(int value) {
    Node* temp = front;
    while (temp) {
      if (temp->data == value) {
        return true;
      }
      temp = temp->next;
    }
    return false;
  }
};

const int floorLEDs[] = { 2, 3, 4 };
const int operationLED = 5;
const int buttonPins[] = { 6, 7, 8 };
const int buzzer = 9;

int currentFloor = 0;
bool isMoving = false;
Queue<int> floorQueue;
volatile bool toggleLED = false;
bool operationLEDState = LOW;

unsigned long lastDebounceTime[3] = {0, 0, 0}; 
const long debounceDelay = 50; 
unsigned long previousMillis = 0;
unsigned long elevatorMoveMillis = 0;
unsigned long blinkDelay = 300;
const long floorDelay = 2000;
bool startMoveDelayOver = false;
unsigned long startMoveDelayStart = 0;
const long startMoveDelayDuration = 2000;



void handleButtonPress(int buttonIndex);

void buttonISR_1() { handleButtonPress(0); }
void buttonISR_2() { handleButtonPress(1); }
void buttonISR_3() { handleButtonPress(2); }

void (*buttonISR[3])() = {buttonISR_1, buttonISR_2, buttonISR_3};

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 3; i++) {
    pinMode(floorLEDs[i], OUTPUT);
  }
  pinMode(operationLED, OUTPUT);

  digitalWrite(floorLEDs[0], HIGH);
  digitalWrite(operationLED, HIGH);

  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), buttonISR[i], RISING);
  }
}


bool pendingMove = false;
int targetFloor = -1;

void loop() {
  for (int i = 0; i < 3; i++) {
    int reading = digitalRead(buttonPins[i]);
    if (reading == LOW) { 
      if ((millis() - lastDebounceTime[i]) > debounceDelay) {
        if (i != currentFloor && !floorQueue.contains(i)){
          floorQueue.enqueue(i);
          Serial.println("Button for floor " + String(i + 1) + " pressed.");
        }
      }
      lastDebounceTime[i] = millis();
    }
  }

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

void handleButtonPress(int buttonIndex) {
  if ((millis() - lastDebounceTime[buttonIndex]) > debounceDelay) {
    floorQueue.enqueue(buttonIndex + 1);
    toggleLED = true;
  }
  lastDebounceTime[buttonIndex] = millis();
}

unsigned long floorPauseStart = 0;
bool inFloorPause = false;

void moveElevatorStep(int targetFloor) {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= blinkDelay) {
    previousMillis = currentMillis;

    if (operationLEDState == LOW) {
      operationLEDState = HIGH;
    } else {
      operationLEDState = LOW;
    }

    digitalWrite(operationLED, operationLEDState);
  }
  if (!startMoveDelayOver && (currentMillis - startMoveDelayStart) < startMoveDelayDuration) {        
    return;
  } else {
    startMoveDelayOver = true;
  }
  tone(buzzer,500);

  int floorDirection = targetFloor > currentFloor ? 1 : -1;

  

  if (inFloorPause) {
    if (currentMillis - floorPauseStart >= floorDelay) {
      inFloorPause = false;
    }
    return;
  }

  if (currentMillis - elevatorMoveMillis >= 4000) {
    digitalWrite(floorLEDs[currentFloor], LOW);
    currentFloor += floorDirection;
    digitalWrite(floorLEDs[currentFloor], HIGH);
    elevatorMoveMillis = currentMillis;
  }
  noTone(buzzer);

  if (currentFloor == targetFloor && !inFloorPause) {
    pendingMove = false;
    isMoving = false;
    noTone(buzzer);
    tone(buzzer, 2000, 100);
    operationLEDState = HIGH;
    digitalWrite(operationLED, operationLEDState);
    inFloorPause = true;
    floorPauseStart = currentMillis;
  }
}
