#include <LiquidCrystal.h>
#include "LedControl.h"
#include <EEPROM.h>
void final();
void highScores();
void moveBullets();
void fireBullet(int dx, int dy);
void updateBullets();
void Menu();
void Game();
void updateMenu();
void drawMap();
void readJoystick();
void animateFallingBlocks();
void movePlayer(int dx, int dy);
void won();
void updateVisibleArea();
void settingsMenu();
void handleSettingsInput();
void handleHighScoresInput();
int readHighScoreFromEEPROM(int address);
void writeHighScoreToEEPROM(int address, int highScore);

// Define the pin connections
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;

// Initialize the LED control
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);

// Player position
int playerX = 0;
int playerY = 0;

int visibleAreaX = 0;  // X-coordinate of the top-left corner of the visible area
int visibleAreaY = 0;  // Y-coordinate of the top-left corner of the visible area

struct GameMap {
  byte* mapData;  // Pointer to the start of the map data
  int mapSize;    // Size of the map (assuming a square map)
};

GameMap currentGameMap;


// Define the map
const int largeMapSize = 21;  // Size of the larger map
byte largeGameMap[largeMapSize][largeMapSize] = {
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 0, 0, 3, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
  { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
  { 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1 },
  { 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1 },
  { 1, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1 },
  { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1 },
  { 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1 },
  { 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1 },
  { 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 1 },
  { 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1 },
  { 1, 0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1 },
  { 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 3, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 }
};

const int maze16Size = 16;
byte mediumGameMap[maze16Size][maze16Size] = {
  { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 3, 3, 3, 3, 0, 3, 1, 1, 0, 3, 3, 0, 3, 0, 1 },
  { 0, 0, 3, 3, 0, 0, 1, 0, 1, 3, 3, 1, 3, 1, 1, 1 },
  { 1, 3, 1, 1, 0, 1, 3, 3, 0, 3, 1, 1, 1, 1, 1, 1 },
  { 1, 0, 0, 1, 0, 0, 1, 1, 1, 3, 0, 1, 0, 0, 1, 1 },
  { 1, 1, 0, 1, 3, 3, 1, 0, 0, 0, 3, 3, 0, 1, 3, 1 },
  { 1, 0, 0, 1, 1, 3, 0, 1, 3, 1, 3, 3, 1, 1, 3, 1 },
  { 1, 0, 1, 0, 3, 1, 3, 1, 1, 3, 1, 1, 1, 0, 1, 1 },
  { 1, 3, 0, 1, 1, 0, 1, 1, 3, 0, 0, 0, 0, 3, 1, 1 },
  { 1, 0, 3, 3, 0, 1, 3, 0, 0, 0, 1, 3, 1, 0, 0, 1 },
  { 1, 1, 0, 1, 1, 3, 0, 3, 3, 1, 3, 3, 0, 3, 0, 1 },
  { 1, 0, 3, 3, 1, 3, 3, 3, 0, 3, 1, 1, 1, 1, 3, 1 },
  { 1, 0, 1, 1, 3, 0, 0, 0, 0, 0, 1, 0, 3, 3, 1, 0 },
  { 1, 3, 0, 1, 0, 0, 3, 1, 3, 3, 0, 0, 1, 3, 0, 0 },
  { 1, 0, 0, 3, 3, 3, 3, 3, 0, 0, 0, 0, 3, 3, 0, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 }
};




const int maze10Size = 10;
byte smallGameMap[maze10Size][maze10Size] = {
  { 0, 0, 1, 1, 1, 1, 1, 1, 1, 1 },
  { 0, 0, 0, 1, 0, 0, 1, 0, 3, 1 },
  { 1, 0, 0, 3, 3, 0, 0, 3, 0, 1 },
  { 1, 1, 3, 0, 1, 1, 1, 1, 3, 1 },
  { 1, 0, 0, 3, 3, 1, 3, 1, 1, 1 },
  { 1, 0, 3, 0, 1, 1, 0, 3, 1, 1 },
  { 1, 3, 3, 3, 1, 0, 1, 0, 1, 1 },
  { 1, 3, 1, 0, 3, 0, 1, 3, 3, 1 },
  { 1, 1, 0, 3, 1, 3, 0, 0, 3, 0 },
  { 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 }
};

int currentMapSize = 0;

const int hardMapSize = 21;
const int mediumMapSize = 16;
const int smallMapSize = 10;

//byte mapValue = currentGameMap.mapData[visibleAreaY + row][visibleAreaX + col];

int winningPosX = largeMapSize - 1;
int winningPosY = largeMapSize - 1;


bool playerMoved = false;

// Blinking control variables
unsigned long previousMillis = 0;  // stores last update time
const long interval = 500;         // interval at which to blink (milliseconds)
bool playerLedState = false;       // current state of the player's LED

// Movement delay variables
const long moveInterval = 200;   // Time in milliseconds between moves
unsigned long lastMoveTime = 0;  // Last time the player moved

unsigned long previousWallMillis = 0;
const long wallInterval = 1;
bool wallLedState = false;

// LCD pins
const int rs = 8, en = 9, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Joystick pins
const int joyX = A1;
const int joyY = A0;
const int joySW = 2;

int menuItem = 0;
const int menuSize = 3;  // Number of menu items
int topItem = 0;         // Top item currently being displayed

// Menu options
const char* menuOptions[menuSize] = { "Start", "Highest Scores", "Settings" };

const long bulletMoveInterval = 100;  // Bullet movement interval in milliseconds


struct Bullet {
  int x, y;
  int dx, dy;                      // Direction of the bullet
  bool active;                     // Is the bullet currently active?
  bool isOn;                       // Track the LED state for blinking
  unsigned long lastMoveTime = 0;  // Last time the bullet moved
};

int elapsedTime = 0;


byte getValueAt(int x, int y) {
  if (x >= 0 && x < currentGameMap.mapSize && y >= 0 && y < currentGameMap.mapSize) {
    return currentGameMap.mapData[y * currentGameMap.mapSize + x];
  }
  return 0;  // Return a default value if out of bounds
}

const int maxBullets = 5;  // Maximum number of bullets at a time
Bullet bullets[maxBullets];

int lastPlayerDX = 0;  // Last horizontal movement direction
int lastPlayerDY = 0;  // Last vertical movement direction


unsigned long bulletPreviousMillis = 0;
const long bulletInterval = 100;  // Faster blink rate for bullets


int currentHighScore;

int highScoreAddress = 0;  // Starting address in EEPROM for high score


void final() {

  if (elapsedTime < currentHighScore || currentHighScore == 0xFFFFFFFF) {
    writeHighScoreToEEPROM(highScoreAddress, elapsedTime);
    currentHighScore = elapsedTime;
  }
  // Display the score on the LCD
  
  lcd.setCursor(0, 0);
  lcd.print("Score obtained:");
  lcd.setCursor(0, 1);
  lcd.print(elapsedTime);
}


void highScores() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highest score:");
  lcd.setCursor(0, 1);
  lcd.print(currentHighScore);
}




void moveBullets() {
  unsigned long currentMillis = millis();
  for (int i = 0; i < maxBullets; i++) {
    if (bullets[i].active && currentMillis - bullets[i].lastMoveTime > bulletMoveInterval) {
      // Update the last move time

      bullets[i].lastMoveTime = currentMillis;

      // Proposed new position for the bullet
      int newX = bullets[i].x + bullets[i].dx;
      int newY = bullets[i].y + bullets[i].dy;
      byte mapValue = getValueAt(newX, newY);


      // Check bounds and wall collision
      if (newX < 0 || newX >= currentGameMap.mapSize || newY < 0 || newY >= currentGameMap.mapSize) {
        bullets[i].active = false;
      } else if (mapValue == 1 || mapValue == 2) {
        bullets[i].active = false;
      } else if (mapValue == 3) {
        currentGameMap.mapData[newY * currentGameMap.mapSize + newX] = 0;  // Corrected array access
        bullets[i].active = false;
      } else {
        bullets[i].x = newX;
        bullets[i].y = newY;
      }
    }
  }
}




void fireBullet(int dx, int dy) {
  if (dx == 0 && dy == 0) return;  // No bullet fired if player hasn't moved

  for (int i = 0; i < maxBullets; i++) {
    if (!bullets[i].active) {
      bullets[i].x = playerX;
      bullets[i].y = playerY;
      bullets[i].dx = dx;
      bullets[i].dy = dy;
      bullets[i].active = true;
      bullets[i].lastMoveTime = millis();  // Initialize the last move time
      break;
    }
  }
}


void updateBullets() {
  unsigned long currentMillis = millis();
  if (currentMillis - bulletPreviousMillis >= bulletInterval) {
    bulletPreviousMillis = currentMillis;
    for (int i = 0; i < maxBullets; i++) {
      if (bullets[i].active) {
        bullets[i].isOn = !bullets[i].isOn;  // Toggle the bullet's state
        lc.setLed(0, bullets[i].y, bullets[i].x, bullets[i].isOn);
      }
    }
  }
}


const int buzzerPin = 7;

void setup() {
  // Initialize LCD
  currentHighScore = readHighScoreFromEEPROM(highScoreAddress);

  lcd.begin(16, 2);
  pinMode(joyX, INPUT);
  pinMode(joyY, INPUT);
  pinMode(joySW, INPUT_PULLUP);  // Enable internal pull-up
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);
  lc.clearDisplay(0);
  pinMode(buzzerPin, OUTPUT);
  // Display initial menu
  largeGameMap[playerY][playerX] = 2;
  mediumGameMap[playerY][playerX] = 2;
  smallGameMap[playerY][playerX] = 2;

  currentGameMap.mapData = &smallGameMap[0][0];  // Flatten the 2D array
  currentGameMap.mapSize = smallMapSize;

  winningPosX = currentGameMap.mapSize - 1;
  winningPosY = currentGameMap.mapSize - 1;

  updateMenu();

  Serial.begin(9600);
}


enum State { MENU,
             HIGHSCORES,
             SETTINGS,
             GAME,
             STARTING,
             WON,
             FINAL,
             SETTINGS_DIFFICULTY };

int difficulty = 0;  // Default difficulty level
const char* difficulties[3] = { "Easy", "Medium", "Hard" };


State currentState = MENU;

void loop() {
  switch (currentState) {
    case MENU:
      Menu();
      break;
    case HIGHSCORES:
      handleHighScoresInput();
      break;
    case SETTINGS_DIFFICULTY:
      handleSettingsInput();
      break;
    case GAME:
      Game();
      break;
    case STARTING:
      animateFallingBlocks();
      break;
    case WON:
      won();
      break;
    case FINAL:
      final();
      break;
  }
}

unsigned long lastJoystickUpdateTime = 0;
const long joystickUpdateInterval = 200;

void Menu() {
  unsigned long currentMillis = millis();

  // Check if the joystick debounce interval has passed
  if (currentMillis - lastJoystickUpdateTime > joystickUpdateInterval) {
    int yValue = analogRead(joyY);
    bool buttonPressed = !digitalRead(joySW);  // Inverted logic due to pull-up

    // Joystick navigation: Up
    if (yValue < 300) {
      menuItem--;
      if (menuItem < 0) menuItem = 0;              // Prevent going above the first item
      if (menuItem < topItem) topItem = menuItem;  // Scroll up
      updateMenu();
      lastJoystickUpdateTime = currentMillis;  // Update the last joystick update time
    }
    // Joystick navigation: Down
    else if (yValue > 700) {
      menuItem++;
      if (menuItem >= menuSize) menuItem = menuSize - 1;   // Prevent going below the last item
      if (menuItem > topItem + 1) topItem = menuItem - 1;  // Scroll down
      updateMenu();
      lastJoystickUpdateTime = currentMillis;  // Update the last joystick update time
    }

    // Select menu item
    if (buttonPressed) {
      tone(buzzerPin, 1000, 200);
      switch (menuItem) {
        case 0:
          currentState = STARTING;  // Start the game
          break;
        case 1:
          currentState = HIGHSCORES;  // Switch to highscore state
          highScores();
          break;
        case 2:
          currentState = SETTINGS_DIFFICULTY;  // Switch to settings state
          settingsMenu();
          break;
      }
      lastJoystickUpdateTime = currentMillis;  // Update the last joystick update time
    }
  }
}

unsigned long gameStartTime;

void Game() {
  unsigned long currentMillis = millis();
  elapsedTime = (currentMillis - gameStartTime) / 1000;  // Convert to seconds

  lcd.clear();
  // Update the LCD with elapsed time
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.setCursor(0, 1);
  lcd.print(elapsedTime);

  // Update blinking only when the player is stationary
  if (!playerMoved && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    playerLedState = !playerLedState;  // Toggle the player's LED state
  } else if (playerMoved) {
    playerLedState = true;  // Keep the player LED on when moving
    playerMoved = false;    // Reset the movement flag
  }

  if (currentMillis - previousWallMillis >= wallInterval) {
    previousWallMillis = currentMillis;
    wallLedState = !wallLedState;
  }

  readJoystick();
  moveBullets();    // Move bullets each game loop iteration
  updateBullets();  // Update bullet blinking
  drawMap();
}

void updateMenu() {
  lcd.clear();
  for (int i = 0; i < 2; i++) {
    int itemIndex = topItem + i;
    if (itemIndex < menuSize) {
      lcd.setCursor(0, i);
      if (itemIndex == menuItem) {
        lcd.print(">");
      } else {
        lcd.print(" ");
      }
      lcd.print(menuOptions[itemIndex]);
    }
  }
}

void drawMap() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      int mapRow = visibleAreaY + row;
      int mapCol = visibleAreaX + col;
      if (mapRow < currentGameMap.mapSize && mapCol < currentGameMap.mapSize) {
        byte mapValue = getValueAt(mapCol, mapRow);

        if (mapValue == 1) {
          lc.setLed(0, row, col, true);  // Wall
        } else if (mapValue == 2) {
          lc.setLed(0, row, col, playerLedState);  // Player, blinking
        } else if (mapValue == 3) {
          lc.setLed(0, row, col, wallLedState);  // Breakable walls blinking
        } else {
          lc.setLed(0, row, col, false);  // Empty space
        }
      }
    }
  }
  for (int i = 0; i < maxBullets; i++) {
    if (bullets[i].active) {
      int visibleBulletX = bullets[i].x - visibleAreaX;
      int visibleBulletY = bullets[i].y - visibleAreaY;
      if (visibleBulletX >= 0 && visibleBulletX < matrixSize && visibleBulletY >= 0 && visibleBulletY < matrixSize) {
        lc.setLed(0, visibleBulletY, visibleBulletX, bullets[i].isOn);
      }
    }
  }
}




void readJoystick() {
  unsigned long currentMillis = millis();
  int xVal = analogRead(joyX);
  int yVal = analogRead(joyY);
  bool buttonPressed = digitalRead(joySW) == LOW;

  // Deadzone threshold
  int deadzone = 300;

  bool joystickMoved = false;  // Flag to check if joystick has moved

  if (xVal < (512 - deadzone) || xVal > (512 + deadzone)) {
    joystickMoved = true;
  }

  if (yVal < (512 - deadzone) || yVal > (512 + deadzone)) {
    joystickMoved = true;
  }

  if (joystickMoved && currentMillis - lastMoveTime > moveInterval) {
    // Reset the direction
    lastPlayerDX = 0;
    lastPlayerDY = 0;

    if (xVal < (512 - deadzone)) {
      movePlayer(-1, 0);
      lastPlayerDX = -1;
    } else if (xVal > (512 + deadzone)) {
      movePlayer(1, 0);
      lastPlayerDX = 1;
    }

    if (yVal > (512 + deadzone)) {
      movePlayer(0, -1);
      lastPlayerDY = -1;
    } else if (yVal < (512 - deadzone)) {
      movePlayer(0, 1);
      lastPlayerDY = 1;
    }

    lastMoveTime = currentMillis;
    playerMoved = true;
  }


  static bool lastButtonState = HIGH;  // Track the last state

  if (buttonPressed && lastButtonState != buttonPressed) {
    fireBullet(lastPlayerDX, lastPlayerDY);
    tone(buzzerPin, 1000, 200);
  }
  lastButtonState = buttonPressed;
  // Fire bullet on button press
  if (buttonPressed && lastButtonState != buttonPressed) {
    // Call fireBullet() with the last known player direction
    fireBullet(lastPlayerDX, lastPlayerDY);
  }

  lastButtonState = buttonPressed;  // Update last button state
}

void animateFallingBlocks() {
  for (int row = matrixSize - 1; row >= 0; row--) {
    for (int col = 0; col < matrixSize; col++) {
      if (row < currentGameMap.mapSize && col < currentGameMap.mapSize) {
        if (getValueAt(col, row) == 1) {

          // Animate each block falling from the top to its place
          for (int fall = 0; fall <= row; fall++) {
            lc.setLed(0, fall, col, true);
            delay(50);
            if (fall != row) {
              lc.setLed(0, fall, col, false);
            }
          }
        }
      }
    }
    gameStartTime = millis();
    currentState = GAME;
  }
}



void movePlayer(int dx, int dy) {
  int newX = playerX + dx;
  int newY = playerY + dy;

  if (newX >= 0 && newX < currentGameMap.mapSize && newY >= 0 && newY < currentGameMap.mapSize && getValueAt(newX, newY) == 0) {
    currentGameMap.mapData[playerY * currentGameMap.mapSize + playerX] = 0;  // Clear old position
    playerX = newX;
    playerY = newY;
    currentGameMap.mapData[playerY * currentGameMap.mapSize + playerX] = 2;  // Set new position
    updateVisibleArea();

    if (playerX == winningPosX && playerY == winningPosY) {
      currentState = WON;
    }
  }

  if (newX != playerX || newY != playerY) {
    playerMoved = true;
  }
  if (playerMoved) {
    lastPlayerDX = dx;
    lastPlayerDY = dy;
    updateVisibleArea();
  }
}

void won() {
  const int numberOfFireworks = 40;  // Total number of fireworks to display
  const int displayTime = 100;       // Time each firework is displayed in milliseconds
  lc.clearDisplay(0);
  for (int i = 0; i < numberOfFireworks; i++) {
    int x = random(matrixSize);
    int y = random(matrixSize);

    // Light up a random LED
    lc.setLed(0, x, y, true);
    delay(displayTime);

    // Turn off the LED
    lc.setLed(0, x, y, false);

    delay(50);
  }
  currentState = FINAL;
  lcd.clear();
}


void updateVisibleArea() {
  // Center the visible area on the player as much as possible
  visibleAreaX = max(0, min(playerX - matrixSize / 2, currentGameMap.mapSize - matrixSize));
  visibleAreaY = max(0, min(playerY - matrixSize / 2, currentGameMap.mapSize - matrixSize));

  // Additional checks for map boundaries
  if (playerX < matrixSize / 2) {
    visibleAreaX = 0;
  } else if (playerX > currentGameMap.mapSize - matrixSize / 2) {
    visibleAreaX = currentGameMap.mapSize - matrixSize;
  }

  if (playerY < matrixSize / 2) {
    visibleAreaY = 0;
  } else if (playerY > currentGameMap.mapSize - matrixSize / 2) {
    visibleAreaY = currentGameMap.mapSize - matrixSize;
  }
}



void settingsMenu() {
  // Display the settings menu
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Difficulty: ");
  lcd.setCursor(0, 1);
  lcd.print(difficulties[difficulty]);
}


void handleSettingsInput() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastJoystickUpdateTime > joystickUpdateInterval) {
    int yValue = analogRead(joyY);
    bool buttonPressed = !digitalRead(joySW);

    // Navigate through difficulty levels
    if (yValue < 300) {
      difficulty = (difficulty + 2) % 3;
      settingsMenu();
    } else if (yValue > 700) {
      difficulty = (difficulty + 1) % 3;
      settingsMenu();  // Update the display
    }

    // Select the difficulty
    if (buttonPressed) {
      currentState = MENU;
      updateMenu();  // Update the display
    }

    lastJoystickUpdateTime = currentMillis;
  }
  switch (difficulty) {
    case 0:                                          // Easy
      currentGameMap.mapData = &smallGameMap[0][0];  // Flatten the 2D array
      currentGameMap.mapSize = smallMapSize;
      break;
    case 1:                                           // Medium
      currentGameMap.mapData = &mediumGameMap[0][0];  // Flatten the 2D array
      currentGameMap.mapSize = mediumMapSize;
      break;
    case 2:                                          // Hard
      currentGameMap.mapData = &largeGameMap[0][0];  // Flatten the 2D array
      currentGameMap.mapSize = hardMapSize;
      break;
  }

  winningPosX = currentGameMap.mapSize - 1;
  winningPosY = currentGameMap.mapSize - 1;
}
void handleHighScoresInput() {
  unsigned long currentMillis = millis();
  if (currentMillis - lastJoystickUpdateTime > joystickUpdateInterval) {
    int yValue = analogRead(joyY);
    bool buttonPressed = !digitalRead(joySW);

    // Check if joystick button is pressed to return to the main menu
    if (buttonPressed) {
      currentState = MENU;
      updateMenu();
    }

    lastJoystickUpdateTime = currentMillis;
  }
}

int readHighScoreFromEEPROM(int address) {
  int highScore = EEPROM.read(address);
  return highScore;
}


void writeHighScoreToEEPROM(int address, int highScore) {
  EEPROM.put(address, highScore);
}
