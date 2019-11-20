#include "Keypad.h"

//Code 0451
char Last4Digits[] = {'0','0','0','0'};
char Loesungen[] = {'0','4','5','1'};


const byte ROWS = 4; // rows
const byte COLS = 3; // columns
//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11, 10, 9}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);


void setup() {
  Serial.begin(9600);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  
}

void loop() {
  char Eingabe = keypad.getKey();
  if(Eingabe){
      Serial.println("Da");
      Serial.println(Eingabe);
      Last4Digits[0] = Last4Digits[1];
      Last4Digits[1] = Last4Digits[2];
      Last4Digits[2] = Last4Digits[3];
      Last4Digits[3] = Eingabe;
      int correctnumbers = 0;
      for(int c = 0;  c <= 3; c++){
          if(Last4Digits[c] == Loesungen[c]){
            correctnumbers++;
            }
        }
      if(correctnumbers == 4){
        digitalWrite(13, HIGH);
        delay(2000);
        digitalWrite(13, LOW);
        }
      else{
        
        }
    }
  
}
