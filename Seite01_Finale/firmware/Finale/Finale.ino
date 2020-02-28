#include "SD.h"
#include <TM1637Display.h>
#include "TMRpcm.h"
#include "SPI.h"

#define CLK 3
#define DIO 2
#define FINSTART 4
#define FINWIN 6
#define SD_ChipSelectPin 10

long timerValue = 0;
long initialSeconds = 600;
long deltaTime = 0;
long secondsPassed = 0;
unsigned long lastMillis = 0;
float timeLoseFactor = 1;
bool gameOver = false;
unsigned long gameOverTime = 0;
bool finalStarted;

TM1637Display display(CLK, DIO);
TMRpcm tmrpcm;

void setup()
{
  Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin))
  {
    Serial.println("SD fail");
    return;
  }
  pinMode(FINSTART, INPUT);
  pinMode(FINWIN, INPUT);
  Serial.println("Playing");
  tmrpcm.setVolume(5);
  tmrpcm.quality(1);

  //tmrpcm.play("PostNoir.wav");

  display.setBrightness(0x0f);
  display.clear();
  timerValue = initialSeconds * 1000;
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  tmrpcm.speakerPin = 9;
  tmrpcm.pause();

}

void loop() {
  deltaTime = millis() - lastMillis;
  lastMillis = millis();
  int lastSecondsPassed = secondsPassed;
  if (!finalStarted) {
    Serial.println("Waiting for Final");
    if (digitalRead(FINSTART)) {
      Serial.println("finalStarted");
      finalStarted = true;
      tmrpcm.play("EndStart.wav");
    }
  }
  if (finalStarted) {
    if (!gameOver) {
      timerValue -= deltaTime * timeLoseFactor;
      secondsPassed = (millis() * timeLoseFactor) / 1000;
      if (digitalRead(FINWIN)) {
        Serial.println("WIN");
        gameOverTime = millis();
        gameOver = true;
        tmrpcm.play("EndWin.wav");
      }
      if (timerValue <= 0) {
        timerValue = 0;
        gameOver = true;
        gameOverTime = millis();
        tmrpcm.play("EndLoose.wav");
      }
    } else if (millis() - gameOverTime > 600000) {
      gameOver = false;
      finalStarted=false;
      display.setBrightness(0x0f);
      display.clear();
      timerValue = initialSeconds * 1000;
      return;
    }
    display.showNumberDecEx(getTimeFormat(timerValue), 0b01000000, true);
  }
  if (!tmrpcm.isPlaying()) {
    tmrpcm.disable();
  }
}

int getTimeFormat(unsigned long milliseconds) {
  unsigned long totalSeconds = milliseconds / 1000;
  unsigned long minuits = totalSeconds / 60;
  unsigned long seconds = totalSeconds % 60;
  return minuits * 100 + seconds;
}
