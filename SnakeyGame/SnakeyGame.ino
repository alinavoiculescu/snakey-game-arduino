#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#define NOTE_B4  494
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_FS5 740
#define NOTE_GS5 831
#define NOTE_B5  988

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
const byte rsPin = 9;
const byte enablePin = 8;
const byte d4Pin = 7;
const byte d5Pin = 6;
const byte d6Pin = 5;
const byte d7Pin = 4;
const byte noOfLcdCols = 16;
const byte noOfLcdRows = 2;
const byte joyPinX = A0;
const byte joyPinY = A1;
const byte joyPinSW = 2;
const byte noOfMenuItems = 5;
const byte buzzerPin = 3;
const char* menuItems[] = {
  "   Start Game   ",
  "   Highscores   ",
  "    Settings    ",
  "     About      ",
  "  How to Play   "
};
const char* settingsMenuItems[] = {
    "   Difficulty   ",
    "     Sound      ",
    "Reset highscores"
};
const uint32_t difficultyIdxEEPROM = 36;
const uint32_t soundIdxEEPROM = 37;
const byte matrixImages[][matrixSize] = {
  // start game
  {
    0b00000000,
    0b00000000,
    0b00001000,
    0b00011000,
    0b00111000,
    0b00011000,
    0b00001000,
    0b00000000
  },
  // highscore
  {
    0b01111110,
    0b01011010,
    0b00011000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b01111110,
    0b01000010
  },
  // settings
  {
    0b00000000,
    0b00101010,
    0b00101010,
    0b00100100,
    0b00100100,
    0b01110100,
    0b01110100,
    0b00000000
  },
  // about
  {
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000,
    0b00011000,
    0b00011000
  },
  // how to play
  {
    0b00011000,
    0b00000000,
    0b00011000,
    0b00110000,
    0b01100000,
    0b01100110,
    0b01100110,
    0b01111110
  },
  // back arrow
  {
    0b00001000,
    0b00001100,
    0b00001110,
    0b11111111,
    0b11111110,
    0b00001100,
    0b00001000,
    0b00000000   
  },
  // checked
  {
    0b00001000,
    0b00011100,
    0b00111110,
    0b01110111,
    0b11100011,
    0b11000000,
    0b10000000,
    0b00000000
  },
  // sad face
  {
    0b00111100,
    0b01000010,
    0b10100101,
    0b10011001,
    0b10000001,
    0b10100101,
    0b01000010,
    0b00111100
  },
  // happy face
  {
    0b00111100,
    0b01000010,
    0b10011001,
    0b10100101,
    0b10000001,
    0b10100101,
    0b01000010,
    0b00111100
  }
};
const byte maxDifficulty = 3;
const byte noHighscores = 6;
const uint32_t blinkingInterval = 500;
const uint32_t lowerThreshold = 400;
const uint32_t upperThreshold = 600;
const uint32_t snakeMaxLength = 64;

LedControl matrix = LedControl(dinPin, clockPin, loadPin, 1);
LiquidCrystal lcd = LiquidCrystal(rsPin, enablePin, d4Pin, d5Pin, d6Pin, d7Pin);
struct Highscore {
  String name;
  String score;
};
struct Snake {
  byte x;
  byte y;
};
unsigned long startTime;
byte settingsMenuItemsNo = 3;
byte snakeHeadX = 0;
byte snakeHeadY = 0;
byte foodX = 0;
byte foodY = 0;
uint32_t score = 0;
byte stopGame = false;
byte joyXIsNeutral = true;
byte joyYIsNeutral = true;
byte menuIdx = 0;
byte buttonPressed = false;
byte settingsIdx = 0;
byte buttonPressedSettings = false;
byte level = 1;
byte buttonPressedExitGame = false;
byte soundOn = 1;
Highscore highscores[noHighscores];
byte blinkingState = true;
unsigned long lastBlinkingTime = 0;
Snake snake[snakeMaxLength];
uint32_t snakeLength = 1;
byte difficulty = 1;
byte up = 0;
byte down = 1;
byte right = 2;
byte left = 3;
byte direction = up;


void setup() {
  pinMode(joyPinSW, INPUT_PULLUP);
  initializeMatrix();
  initializeLcd();
  printGreetingMessage();
  readHighscores();
  readSound();
  readDifficulty();
  Serial.begin(9600);
}

void loop() {
  menuLoop();
}

void initializeMatrix() {
  const uint32_t intensity = 8;
  byte status = false;  
  matrix.shutdown(0, status);
  matrix.setIntensity(0, intensity);
  matrix.clearDisplay(0);
}

void initializeLcd() {
  lcd.begin(noOfLcdCols, noOfLcdRows);
  lcd.clear();
}

void printGreetingMessage() {
  lcd.setCursor(0, 0);
  lcd.print("   Welcome to   ");
  lcd.setCursor(0, 1);
  lcd.print("sSsSsSsSsSNAKEY!");
  startGameSound();
}

void readHighscores() {
  uint32_t positionCount = 0;
  for (uint32_t i = 0; i < noHighscores; i++) {
    highscores[i].name = (char)EEPROM.read(positionCount);
    highscores[i].name += (char)EEPROM.read(positionCount + 1);
    highscores[i].name += (char)EEPROM.read(positionCount + 2);
    highscores[i].name += (char)EEPROM.read(positionCount + 3);
    highscores[i].score = (char)EEPROM.read(positionCount + 4);
    highscores[i].score += (char)EEPROM.read(positionCount + 5);
    positionCount += 6;
  }
}

void resetHighscores() {
  const unsigned long millisToWait = 1000;
  uint32_t positionCount = 0;
  for (uint32_t i = 0; i < noHighscores; i++) {
    EEPROM.update(positionCount, 'N');
    EEPROM.update(positionCount + 1, 'U');
    EEPROM.update(positionCount + 2, 'L');
    EEPROM.update(positionCount + 3, 'L');
    EEPROM.update(positionCount + 4, '0');
    EEPROM.update(positionCount + 5, '0');
    positionCount += 6;
  }
  readHighscores();
  showMatrixImage(noOfMenuItems + 1);
  wait(millisToWait);
  showMatrixImage(noOfMenuItems);
}

void wait(unsigned long milliseconds) {
  const unsigned long startTimeWait = millis();
  while (millis() - startTimeWait < milliseconds) { }
}

void printMenuItem() {
  lcd.setCursor(0, 0);
  lcd.print(menuItems[menuIdx]);
  showMatrixImage(menuIdx);
  lcd.setCursor(0, 1);
  if (menuIdx == 0) {
    lcd.print("<---------------");
  }
  else if (menuIdx == noOfMenuItems - 1) {
    lcd.print("--------------->");
  }
  else {
    lcd.print("<-------------->");
  }
}

void scrollMenuItems() {
  const uint32_t joyValueY = analogRead(joyPinY);
  if (joyValueY < lowerThreshold && joyYIsNeutral && menuIdx > 0) {
    joyYIsNeutral = false;
    menuIdx--;
    optionChangeSound();
  } else if (joyValueY > upperThreshold && joyYIsNeutral && menuIdx < noOfMenuItems - 1) {
    joyYIsNeutral = false;
    menuIdx++;
    optionChangeSound();
  }
  else if (joyValueY >= lowerThreshold && joyValueY <= upperThreshold) {
    joyYIsNeutral = true;
  }
}

void showHighscores() {
  byte highscoreIdx = 0;
  lcd.clear();
  showMatrixImage(noOfMenuItems);
  do {
    printHighscores(highscoreIdx);
    scrollHighscores(highscoreIdx);
  } while (!checkIfGoBack());
}

byte checkIfGoBack() {
  const uint32_t joyValueY = analogRead(joyPinY);
  if (joyValueY > upperThreshold && joyYIsNeutral) {
    return true;
  }
  return false;
}

void printHighscores(byte &highscoreIdx) {
  const String stringToDisplay = getHighscore(highscoreIdx);
  if (highscoreIdx == 0) {
    scrollTextOnDisplayLine(stringToDisplay + " ", 0);
  }
  else {
    scrollTextOnDisplayLine(stringToDisplay + "^", 0);
  }
  if (highscoreIdx == noHighscores - 1) {
    scrollTextOnDisplayLine("SCROLL FOR MORE ", 1);
  }
  else {
    scrollTextOnDisplayLine("SCROLL FOR MOREv", 1);
  }
}

void scrollHighscores(byte &highscoreIdx) {
  scrollItemsUpDown(highscoreIdx, noHighscores);
}

String getHighscore(uint32_t position) {
  return (String)"  " + (position + 1) + ". " + highscores[position].name + " " + highscores[position].score + "   ";
}

void printCurrentDifficulty() {
  lcd.setCursor(0, 0);
  lcd.print("   Current: ");
  lcd.print(difficulty + 1);
  if (difficulty != 0) {
    lcd.print("  ^");
  }
  else {
    lcd.print("   ");
  }
  lcd.setCursor(0, 1);
  switch (difficulty) {
    case 0:
      lcd.print("      EASY     ");
      break;
    case 1:
      lcd.print("     MEDIUM    ");
      break;
    case 2:
      lcd.print("      HARD     ");
      break;
  }
  if (difficulty != 2) {
    lcd.print("v");
  }
  else {
    lcd.print(" ");
  }
}

byte scrollDifficulty() {
  scrollItemsUpDown(difficulty, maxDifficulty);
  return difficulty;
}

void updateDifficulty() {
  do {
    difficulty = scrollDifficulty();
    printCurrentDifficulty();
  } while (!checkIfGoBack());
  EEPROM.update(difficultyIdxEEPROM, difficulty);
}

void printCurrentSoundSetting() {
  lcd.setCursor(0, 0);
  if (soundOn == false) {
    lcd.print("    Volume:     ");
  }
  else {
    lcd.print("    Volume:    ^");
  }
  lcd.setCursor(0, 1);
  switch (soundOn) {
    case 0:
      lcd.print("      OFF      ");
      break;
    case 1:
      lcd.print("       ON      ");
      break;
  }
  if (soundOn == true) {
    lcd.print(" ");
  }
  else {
    lcd.print("v");
  }
}

void updateSound() {
  byte maxSound = 1;
  do {
    scrollItemsUpDown(soundOn, maxSound + 1);
    printCurrentSoundSetting();
  } while (!checkIfGoBack());
  EEPROM.update(soundIdxEEPROM, soundOn);
}

void showSettings() {
  const unsigned long millisToWait = 100;
  showMatrixImage(noOfMenuItems);
  do {
    printSettingsItem();
    scrollSettingsItems();
    wait(millisToWait);
    checkIfSettingsOptionSelected();
  } while (!checkIfGoBack());
}

void printSettingsItem() {
  scrollTextOnDisplayLine(settingsMenuItems[settingsIdx], 0);
  scrollTextOnDisplayLine("  SCROLL/CLICK  ", 1);  
}

void scrollSettingsItems() {
  scrollItemsUpDown(settingsIdx, settingsMenuItemsNo);
}

void scrollItemsUpDown(byte &idx, byte maxIdx) {
  const uint32_t joyValueX = analogRead(joyPinX);
  if (joyValueX < lowerThreshold && joyXIsNeutral && idx > 0) {
    joyXIsNeutral = false;
    idx--;
    optionChangeSound();
  } else if (joyValueX > upperThreshold && joyXIsNeutral && idx < maxIdx - 1) {
    joyXIsNeutral = false;
    idx++;
    optionChangeSound();
  }
  else if (joyValueX > lowerThreshold && joyValueX < upperThreshold) {
    joyXIsNeutral = true;
  }   
}

void checkIfSettingsOptionSelected() {
  const unsigned long millisToWait = 100;
  const uint32_t joySWCurrentState = digitalRead(joyPinSW);
  if (joySWCurrentState == LOW && !buttonPressedSettings) {
    buttonPressedSettings = true;
    if (settingsIdx == 0) {
      showDifficultySettings();
    } else if (settingsIdx == 1) {
      showSoundSettings();
    } else if (settingsIdx == 2) {
      resetHighscores();
    }
    wait(millisToWait);
    buttonPressedSettings = false;
    showSettings();
  }
}

void readDifficulty() {
  difficulty = EEPROM.read(difficultyIdxEEPROM);
}

void readSound() {
  soundOn = EEPROM.read(soundIdxEEPROM);  
}

void showDifficultySettings() {
  lcd.clear();
  do {
    printCurrentDifficulty();
    updateDifficulty();
  } while (!checkIfGoBack());
}

void showSoundSettings() {
  lcd.clear();
  do {
    updateSound();
  } while (!checkIfGoBack());
}

void showAboutInfo() {
  lcd.clear();
  showGameAndCreator();
  showGitHub();
}

void showGameAndCreator() {
  lcd.setCursor(0, 0);
  scrollTextOnDisplayLine("Snakey v1.0 created by Alina Voiculescu", 0);
}

void showGitHub() {
  const unsigned long millisToWaitForUser = 600;
  const unsigned long millisToWaitForRepo = 3000;
  lcd.setCursor(0, 1);
  scrollTextOnDisplayLine("GitHub user: alinavoiculescu", 1);
  wait(millisToWaitForUser);
  scrollTextOnDisplayLine("GitHub repo: snakey-game-arduino", 1);
  wait(millisToWaitForRepo);
}

void showInstructions() {
  const unsigned long millisToWait = 3000;
  lcd.clear();
  lcd.setCursor(0, 0);
  scrollTextOnDisplayLine("Use the joystick to collect food", 0);
  lcd.setCursor(0, 1);
  scrollTextOnDisplayLine("Get the best highscore ever!", 1);
  wait(millisToWait);
}

void checkIfOptionSelected() {
  const uint32_t joySWCurrentState = digitalRead(joyPinSW);
  if (joySWCurrentState == LOW && !buttonPressed) {
    buttonPressed = true;
    if (menuIdx == 0) {
      gameLoop();
    } else if (menuIdx == 1) {
      showHighscores();
    } else if (menuIdx == 2) {
      showSettings();
    } else if (menuIdx == 3) {
      showAboutInfo();
    } else if (menuIdx == 4) {
      showInstructions();
    }
  }
  buttonPressed = false;
  resetGame();
}

void resetGame() {
  stopGame = false;
  snakeHeadX = 0;
  snakeHeadY = 0;
  foodX = 0;
  foodY = 0;
  score = 0;
  level = 1;
  buttonPressedSettings = false;
  buttonPressedExitGame = false;
  snakeLength = 1;
  direction = 1;
  Snake newSnake[snakeMaxLength];
  for (uint32_t i = 0; i < snakeMaxLength; i++) {
    snake[i] = newSnake[i];
  }
}

void showMatrixImage(byte idx) {
  matrix.clearDisplay(0);
  for (uint32_t i = 0; i < matrixSize; i++) {
    matrix.setRow(0, i, matrixImages[idx][i]);
  }
}

void menuLoop() {
  printMenuItem();
  scrollMenuItems();
  checkIfOptionSelected();
}

void generateFood() {
  byte collides;  
  do {
    foodX = random(0, matrixSize);
    foodY = random(0, matrixSize);
    collides = false;
    for (uint32_t i = 0; i < snakeLength; i++) {
      if (foodX == snake[i].x && foodY == snake[i].y) {
        collides = true;
        break;
      }
    }
    collides = collidesWithWalls(foodX, foodY, true);
  } while (collides);
}

byte collidesWithWalls(byte x, byte y, byte isFood) {
  byte collides = false;
  if (level >= 2  || (isFood && level == 1 && score == 2)) {
    for (uint32_t i = 0; i < matrixSize; i++) {
      if ((x == 0 && y == i) || (x == matrixSize - 1 && y == i)) {
        collides = true;
        break;
      }
      if ((level == 3 || (isFood && level == 2 && score == 4)) && ((x == i && y == 0) || (x == i && y == matrixSize - 1))) {
        collides = true;
        break;
      }
    }
  }
  return collides;
}

byte bitesHimself() {
  if (snakeLength != 1 && snakeHeadX == snake[snakeLength - 1].x && snakeHeadY == snake[snakeLength - 1].y) {
    return true;
  }
  return false;
}

void generatePlayerInitialPosition() {
  snakeHeadX = random(0, matrixSize);
  snakeHeadY = random(0, matrixSize);
  while (snakeHeadX == foodX  && snakeHeadY == foodY) {
    snakeHeadX = random(0, matrixSize);
    snakeHeadY = random(0, matrixSize);
  }
  snake[0].x = snakeHeadX;
  snake[0].y = snakeHeadY;
}

uint32_t setSpeed() {
  const uint32_t speedEasy = 500;
  const uint32_t speedMedium = 300;
  const uint32_t speedHard = 100;
  uint32_t speed = 0;
  switch (difficulty) {
    case 0:
      speed = speedEasy;
      break;
    case 1:
      speed = speedMedium;
      break;
    case 2:
      speed = speedHard;
      break;
  }
  return speed;
}

void showInitialGameLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  lcd.setCursor(7, 0);
  lcd.print(" Score:");
  lcd.print(0);
  lcd.print(score);
  lcd.setCursor(0, 1);
  lcd.print("PRESS 2s TO EXIT");
}

void updatePlayerPosition() {
  const uint32_t joyValueX = analogRead(joyPinX);
  const uint32_t joyValueY = analogRead(joyPinY);
  if (joyValueX < lowerThreshold && (snakeLength == 1 || direction != up)) {
    direction = down;
  } else if (joyValueX > upperThreshold && (snakeLength == 1 || direction != down)) {
    direction = up;
  } else if (joyValueY < lowerThreshold && (snakeLength == 1 || direction != left)) {
    direction = right;
  } else if (joyValueY > upperThreshold && (snakeLength == 1 || direction != 2)) {
    direction = left;
  }
  moveToDirection();
}

void moveToDirection() {
  if (direction == up) {    
    if (snakeHeadX == 0) {
      snakeHeadX = matrixSize - 1;
    }
    else {
      snakeHeadX--;
    }

  }
  else if (direction == down) {
    if (snakeHeadX == matrixSize - 1) {
      snakeHeadX = 0;
    }
    else {
      snakeHeadX++;
    }
  }
  else if (direction == right) {
    if (snakeHeadY == 0) {
      snakeHeadY = matrixSize - 1;
    }
    else {
      snakeHeadY--;
    }
  }
  else if (direction == left) {
    if (snakeHeadY == matrixSize - 1) {
      snakeHeadY = 0;
    }
    else {
      snakeHeadY++;
    }
  }
  updateSnakeBody();
  snake[0].x = snakeHeadX;
  snake[0].y = snakeHeadY;
  showSnake();
  if(collidesWithWalls(snakeHeadX, snakeHeadY, false) || bitesHimself() || pressForExitGame()) {
    endGame();
  }  
}

byte pressForExitGame() {
  const unsigned long millisToWait = 300;
  const unsigned long millisPressedForExit = 2000;
  wait(millisToWait);
  const uint32_t joySWCurrentState = digitalRead(joyPinSW);
  if (joySWCurrentState == LOW && !buttonPressedExitGame) {
    buttonPressedExitGame = true;
    startTime = millis();
  }
  else if (joySWCurrentState == HIGH) {
    buttonPressedExitGame = false;    
  }
  if (buttonPressedExitGame && millis() - startTime >= millisPressedForExit) {
    return true;
  }
  return false;
}

void endGame() {
  const unsigned long millisToWait = 3000;
  showMatrixImage(noOfMenuItems + 2);
  lcd.clear();
  scrollTextOnDisplayLine(" GAME OVER! :(  ", 0);
  descendingNotes();
  wait(millisToWait);
  handleHighscore();
}

void handleHighscore() {
  const uint32_t maxScoreOneDigit = 9;
  byte isHighscore = false;
  byte place;
  String newScore = (String)score;
  if (score <= maxScoreOneDigit) {
      newScore = "0" + (String)score;
  }
  else {
      newScore = score;
  }
  if (newScore > highscores[0].score) {
    isHighscore = true;
    place = 1;
    highscores[5] = highscores[4];
    highscores[4] = highscores[3];
    highscores[3] = highscores[2];
    highscores[2] = highscores[1];
    highscores[1] = highscores[0];
  }
  else if (newScore > highscores[1].score) {
    isHighscore = true;
    place = 2;
    highscores[5] = highscores[4];
    highscores[4] = highscores[3];
    highscores[3] = highscores[2];
    highscores[2] = highscores[1];
  }
  else if (newScore > highscores[2].score) {
    isHighscore = true;
    place = 3;
    highscores[5] = highscores[4];
    highscores[4] = highscores[3];
    highscores[3] = highscores[2];
  }
  else if (newScore > highscores[3].score) {
    isHighscore = true;
    place = 4;
    highscores[5] = highscores[4];
    highscores[4] = highscores[3];
  }
  else if (newScore > highscores[4].score) {
    isHighscore = true;
    place = 5;
    highscores[5] = highscores[4];
  }
  else if (newScore > highscores[5].score) {
    isHighscore = true;
    place = 6;
  }

  if (isHighscore) {
    const unsigned long millisToWait = 3000;
    lcd.clear();
    showMatrixImage(noOfMenuItems + 3);
    scrollTextOnDisplayLine(" NEW HIGHSCORE! ", 0);
    scrollTextOnDisplayLine((String)" YOUR PLACE: " + place, 1);
    ascendingNotes();
    wait(millisToWait);
    String name = (String)requestUserName();
    showMatrixImage(noOfMenuItems + 1);
    optionChangeSound();
    highscores[place - 1].name = (String)name;
    highscores[place - 1].score = newScore;
    updateHighscores();
  }
  stopGame = true;
}

void updateHighscores() {
  uint32_t positionCount = 0;
  byte currentLetter;
  byte currentDigit;
  for (uint32_t i = 0; i < noHighscores; i++) {
    currentLetter = 0;
    currentDigit = 0;
    EEPROM.update(positionCount, highscores[i].name[currentLetter++]);
    EEPROM.update(positionCount + 1, highscores[i].name[currentLetter++]);
    EEPROM.update(positionCount + 2, highscores[i].name[currentLetter++]);
    EEPROM.update(positionCount + 3, highscores[i].name[currentLetter++]);
    EEPROM.update(positionCount + 4, highscores[i].score[currentDigit++]);
    EEPROM.update(positionCount + 5, highscores[i].score[currentDigit++]);
    positionCount += 6;
  }
}

String requestUserName() {
  const uint32_t minAlphabetLetter = 65;
  const uint32_t maxAlphabetLetter = 90;
  const byte minNamePosition = 6;
  const byte maxNamePosition = 9;
  const byte maxNameLetters = 4;
  byte nameLetterIdx = minNamePosition;
  byte nameLetters[maxNameLetters] = {minAlphabetLetter, minAlphabetLetter, minAlphabetLetter, minAlphabetLetter};
  String name;
  lcd.clear();
  scrollTextOnDisplayLine("Enter your name:", 0);
  scrollTextOnDisplayLine(" -->  AAAA  <-- ", 1);
  // Change default name letters until player clicks on the button to save the name
  while (true) {
    const uint32_t joySWCurrentState = digitalRead(joyPinSW);
    const uint32_t joyValueX = analogRead(joyPinX);
    const uint32_t joyValueY = analogRead(joyPinY);
    // If button clicked => go back
    if (joySWCurrentState == LOW) {
      break;
    }
    // Show name on LCD
    lcd.setCursor(nameLetterIdx, 1);
    lcd.print((char)nameLetters[nameLetterIdx - minNamePosition]);
    // Change name letter
    if (joyValueX > upperThreshold && joyXIsNeutral && nameLetters[nameLetterIdx - minNamePosition] > minAlphabetLetter) {
      joyXIsNeutral = false;
      nameLetters[nameLetterIdx - minNamePosition]--;
    } else if (joyValueX < lowerThreshold && joyXIsNeutral && nameLetters[nameLetterIdx - minNamePosition] < maxAlphabetLetter) {
      joyXIsNeutral = false;
      nameLetters[nameLetterIdx - minNamePosition]++;
    }
    else if (joyValueX >= lowerThreshold && joyValueX <= upperThreshold) {
      joyXIsNeutral = true;
    }
    // Change name letter position
    if (joyValueY < lowerThreshold && joyYIsNeutral && nameLetterIdx < maxNamePosition) {
      joyYIsNeutral = false;
      nameLetterIdx++;
    } else if (joyValueY > upperThreshold && joyYIsNeutral && nameLetterIdx > minNamePosition) {
      joyYIsNeutral = false;
      nameLetterIdx--;
    }
    else if (joyValueY >= lowerThreshold && joyValueY <= upperThreshold) {
      joyYIsNeutral = true;
    }
  }
  // Concatenate name letters
  for (uint32_t i = 0; i < maxNameLetters; i++) {
    name += (char)nameLetters[i];
  }
  return name;
}

void updateSnakeBody() {
  for (uint32_t i = snakeLength - 1; i > 0; i--) {
    snake[i].x = snake[i - 1].x;
    snake[i].y = snake[i - 1].y;
  }
}

void showSnake() {
  for (uint32_t i = 0; i < snakeLength; i++) {
    matrix.setLed(0, snake[i].x, snake[i].y, true);
  }
}

void updateGameLcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  lcd.setCursor(7, 0);
  lcd.print(" Score:");
  if (((String)score).length() == 1) {
    lcd.print(0);
    lcd.print(score);
  }
  else {
    lcd.print(score);
  }
  lcd.setCursor(0, 1);
  lcd.print("PRESS 2s TO EXIT");
}

void checkIfFoodCollected() {
  if (snakeHeadX == foodX && snakeHeadY == foodY) {
      grow();
      increaseScore();
      increaseScoreSound();      
      checkIfLevelUp();
      updateGameLcd();
      generateFood();
  }
}

void increaseScore() {
  const byte scoreToIncreaseEasy = 1;
  const byte scoreToIncreaseMedium = 2;
  const byte scoreToIncreaseHard = 3;
  switch (difficulty) {
    case 0:
      score += scoreToIncreaseEasy;
      break;
    case 1:
      score += scoreToIncreaseMedium;
      break;
    case 2:
      score += scoreToIncreaseHard;
      break;
  }  
}

void grow() {
  snakeLength++;
  snake[snakeLength - 1] = snake[snakeLength - 2];
}

void gameLoop() {
  const uint32_t speed = setSpeed();
  score = 0;
  level = 1;
  generatePlayerInitialPosition();
  matrix.setLed(0, snake[0].x, snake[0].y, true);
  generateFood();
  blinkFood();
  showInitialGameLcd();
  while (!stopGame) {
    matrix.clearDisplay(0);
    addWalls();
    updatePlayerPosition();
    blinkFood();
    checkIfFoodCollected();
    wait(speed);
  }
}

void checkIfLevelUp() {
  uint32_t scoreLevel1 = 0;
  uint32_t scoreLevel2 = 0;
  uint32_t scoreLevel3 = 0;
  switch (difficulty) {
    case 0:
      scoreLevel1 = 15;
      scoreLevel2 = 25;
      scoreLevel3 = 33;
      break;
    case 1:
      scoreLevel1 = 30;
      scoreLevel2 = 50;
      scoreLevel3 = 66;
      break;
    case 2:
      scoreLevel1 = 45;
      scoreLevel2 = 75;
      scoreLevel3 = 99;
      break;    
  }
  if ((level == 1 && score == scoreLevel1) || (level == 2 && score == scoreLevel2)) {
    lcd.clear();    
    scrollTextOnDisplayLine((String)"FINISHED LVL " + level + "!", 0);
    level++;
    scrollTextOnDisplayLine((String)"STARTING LVL " + level + "..", 1);
    ascendingNotes();
    wait(3000);
  }
  else if (level == 3 && score == scoreLevel3) {
    winGame();
  }
}

void winGame() {
  showMatrixImage(noOfMenuItems + 3);
  lcd.clear();
  scrollTextOnDisplayLine("CONGRATULATIONS", 0);
  scrollTextOnDisplayLine("'U WON THE GAME!", 1);
  ascendingNotes();
  wait(3000);
  handleHighscore();
}

void addWalls() {
  if (level >= 2) {
    matrix.setRow(0, 0, B11111111);
    matrix.setRow(0, matrixSize - 1, B11111111);  
  }
  if (level == 3) {
    matrix.setColumn(0, 0, B11111111);
    matrix.setColumn(0, matrixSize - 1, B11111111); 
  }
}

void ascendingNotes() {
  if (soundOn) {
    const unsigned long millisNote1 = 200;
    const unsigned long millisNote2 = 300;
    const unsigned long millisNote3 = 400;
    tone(buzzerPin, NOTE_DS5, millisNote1);
    wait(millisNote1);
    tone(buzzerPin, NOTE_E5, millisNote2);
    wait(millisNote2);
    tone(buzzerPin, NOTE_GS5, millisNote3);
    wait(millisNote3);
  }
}

void descendingNotes() {
  if (soundOn) {
    const unsigned long millisNote1 = 200;
    const unsigned long millisNote2 = 300;
    const unsigned long millisNote3 = 400;
    tone(buzzerPin, NOTE_GS5, millisNote1);
    wait(millisNote1);
    tone(buzzerPin, NOTE_E5, millisNote2);
    wait(millisNote2);
    tone(buzzerPin, NOTE_DS5, millisNote3);
    wait(millisNote3);
  }
}

void startGameSound() {
  if (soundOn) {
    const unsigned long millisNote1 = 200;
    const unsigned long millisNote2 = 200;
    const unsigned long millisNote3 = 150;
    const unsigned long millisNote4 = 200;
    const unsigned long millisUntilGameStarts = 3000 - millisNote1 - millisNote2 - millisNote3;
    tone(buzzerPin, NOTE_B4, millisNote1);
    wait(millisNote1);
    tone(buzzerPin, NOTE_B5, millisNote2);
    wait(millisNote2);
    tone(buzzerPin, NOTE_FS5, millisNote3);
    wait(millisNote3);
    tone(buzzerPin, NOTE_FS5, millisNote3);
    wait(millisNote3);
    tone(buzzerPin, NOTE_E5, millisNote4);
    wait(millisNote4);
    tone(buzzerPin, NOTE_E5, millisNote4);
    wait(millisUntilGameStarts);
  }  
}

void increaseScoreSound() {
  if (soundOn) {
    const unsigned long millisNote = 125;
    tone(buzzerPin, NOTE_GS5, millisNote);
    wait(millisNote);
  }
}

void optionChangeSound() {
  if (soundOn) {
    const unsigned long millisNote = 125;
    tone(buzzerPin, NOTE_E5, millisNote);
    wait(millisNote);
  }
}

void scrollTextOnDisplayLine(String text, byte line) {
  const uint32_t maxPossibleLetters = 39;
  const unsigned long millisToScrollLeft = 600;
  const unsigned long millisToScrollRight = 200;
  uint32_t textLength = text.length();
  lcd.setCursor(0, line);
  if (textLength <= noOfLcdCols) {
    lcd.print(text);
  }
  else if (textLength > maxPossibleLetters) {
    lcd.print(text.substring(0, maxPossibleLetters + 1));
    while (textLength != 0) {
      wait(millisToScrollLeft);
      lcd.scrollDisplayLeft();
      textLength--;
    }
    lcd.setCursor(0, line);
    lcd.print(text.substring(maxPossibleLetters + 1));
    textLength = text.length() - (maxPossibleLetters + 2);
    while (text.length() > 0) {
      lcd.scrollDisplayLeft();
      wait(millisToScrollLeft);
      textLength--;
    }
  }
  else if (textLength <= maxPossibleLetters) {
    lcd.print(text);
    while (textLength > noOfLcdCols) {
      wait(millisToScrollLeft);
      lcd.scrollDisplayLeft();
      textLength--;
    }
    while (textLength != text.length()) {
      wait(millisToScrollRight);
      lcd.scrollDisplayRight();
      textLength++;
    }  
  }
}

void blinkFood() {
  if (millis() - lastBlinkingTime >= blinkingInterval) {
    blinkingState = !blinkingState;
    lastBlinkingTime = millis();
  }
  matrix.setLed(0, foodX, foodY, blinkingState);
}