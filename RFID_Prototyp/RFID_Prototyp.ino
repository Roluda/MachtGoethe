/*
   --------------------------------------------------------------------------------------------------------------------
   Example sketch/program showing how to read new NUID from a PICC to serial.
   --------------------------------------------------------------------------------------------------------------------
   This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid

   Example sketch/program showing how to the read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
   Reader on the Arduino SPI interface.

   When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
   then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
   you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
   will show the type, and the NUID if a new card has been detected. Note: you may see "Timeout in communication" messages
   when removing the PICC from reading distance too early.

   @license Released into the public domain.

   Typical pin layout used:
   -----------------------------------------------------------------------------------------
               MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
               Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
   Signal      Pin          Pin           Pin       Pin        Pin              Pin
   -----------------------------------------------------------------------------------------
   SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
   SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
   SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
*/

#include <SPI.h>
#include <MFRC522.h>

const int rstPins[] = {2, 3};
const int ssPins[] = {7, 8};
const int readers = 2;


MFRC522::MIFARE_Key key;

MFRC522 rfid[readers];
int multiplexIndex = 0;


// Init array that will store new NUID
byte nuidPICC[4];

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  Serial.println("Setting up");
  for (int i = 0; i < readers; i++) {
    MFRC522 newRFID(ssPins[i], rstPins[i]);
    rfid[i] = newRFID;
  }

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {
  ResetAll();
  delay(100);
  NextMultiplex();
  //delay(100);
  rfid[multiplexIndex].PCD_Init();
  if (! rfid[multiplexIndex].PICC_IsNewCardPresent() || rfid[multiplexIndex].PICC_ReadCardSerial()) return;

  Serial.println("currentReader: "+ String(multiplexIndex));
  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid[multiplexIndex].PICC_GetType(rfid[multiplexIndex].uid.sak);
  Serial.println(rfid[multiplexIndex].PICC_GetTypeName(piccType));
}

void ResetAll() {
  for (int i = 0; i < readers; i++) {
    digitalWrite(rstPins[i], LOW);
  }
}

void NextMultiplex() {
  if (multiplexIndex < readers - 1) {
    multiplexIndex++;
  } else {
    multiplexIndex = 0;
  }
  for (int i = 0; i < readers; i++) {
    if (i == multiplexIndex) {
      //digitalWrite(rstPins[i], HIGH);
    }
  }
}


/**
   Helper routine to dump a byte array as hex values to Serial.
*/
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
   Helper routine to dump a byte array as dec values to Serial.
*/
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
    Serial.println();
  }
}
