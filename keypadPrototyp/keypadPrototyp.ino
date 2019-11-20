#include "Adafruit_Keypad.h"

#define BoxRelais A0
#define DoneOutput 1

const byte ROWS = 4; // rows
const byte COLS = 3; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 10, 9}; //connect to the column pinouts of the keypad


char currentInput[4];
char targetInput[] = {'0', '4', '5', '1'};
int currentInputIndex;
int inputSize = 4;
bool inputsAccepted = false;
unsigned long lastButtonPress;
unsigned long maxIdleTime = 8000;

//initialize an instance of class NewKeypad
Adafruit_Keypad customKeypad = Adafruit_Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(BoxRelais, OUTPUT);
  pinMode(DoneOutput, OUTPUT);
  Serial.begin(9600);
  customKeypad.begin();

}

void loop() {
  // put your main code here, to run repeatedly:
  customKeypad.tick();
  while (customKeypad.available()) {
    keypadEvent e = customKeypad.read();
    if (e.bit.EVENT == KEY_JUST_PRESSED) {
      if ((char)e.bit.KEY == '*') {
        //startInput();
      } else if ((char)e.bit.KEY == '#') {
        //confirmInput();
      } else {
        addCharToInputArray((char)e.bit.KEY);
        if (inputsMatch()) {
          digitalWrite(DoneOutput, HIGH);
          OpenBox();
        }
      }
    }
  }
  delay(10);
}

void addCharToInputArray(char newInput) {
  for (int i = 0; i < inputSize - 1; i++) {
    currentInput[i] = currentInput[i + 1];
  }
  currentInput[inputSize-1] = newInput;
}

bool inputsMatch() {
  for (int i = 0; i < inputSize; i++) {
    if (currentInput[i] != targetInput[i]) {
      return false;
    }
  }
  return true;
}

void OpenBox() {
  digitalWrite(BoxRelais, HIGH);
  delay(5000);
  digitalWrite(BoxRelais, LOW);
}

void confirmInput() {
  if (!inputFull()) {
    Serial.println("insufficient Inputs");
    resetInput();
    return;
  }
  if (inputsMatch()) {
    Serial.println("OPENED THE GATE");
    OpenBox();
    resetInput();
  } else {
    Serial.println("WRONG INPUT");
    resetInput();
  }
}

void addCharToInput(char c) {
  if (inputFull() || !inputsAccepted) {
    return;
  }
  currentInput[currentInputIndex] = c;
  currentInputIndex++;
}

void startInput() {
  Serial.println("startet Input");
  currentInputIndex = 0;
  inputsAccepted = true;
}

void resetInput() {
  currentInputIndex = 0;
  inputsAccepted = false;
}

bool inputFull() {
  return !(currentInputIndex < inputSize);
}



