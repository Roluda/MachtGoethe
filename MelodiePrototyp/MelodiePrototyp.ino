/*
  Melody

  Plays a melody

  circuit:
  - 8 ohm speaker on digital pin 8

  created 21 Jan 2010
  modified 30 Aug 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Tone
*/

#define BUTTON1 2

#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_FS4, NOTE_FS4, NOTE_A4, NOTE_G4, NOTE_FS4,   NOTE_E4, NOTE_E4, NOTE_E4, 0,    NOTE_FS4, NOTE_FS4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_B4, 0, NOTE_A4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 8, 8,   4, 8, 4, 8,     4, 8, 8, 8, 8, 4, 8, 4
};

bool inMelody;

void setup() {
  pinMode(BUTTON1, INPUT_PULLUP);
  
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 17; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 2000 / noteDurations[thisNote];
    tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(8);
  }
}

void loop() {
  if(digitalRead(BUTTON1)==HIGH){
    //Function(Melodie1);
  }
  // no need to repeat the melody.
}


//Funktion Ton abspielen (int array Melodie, int array tonlänegen, int töne)
//Funktion wie im Setup
