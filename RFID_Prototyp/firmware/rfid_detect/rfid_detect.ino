//=============================================================================
// Exit the library (Siegel 7)
// RFDI detection
// Detect the presence 6 (or 5) objects with RFID tags at the respective RFID-readers.
//=============================================================================

// Compile for 
//   Board: Ardunino Nano
//   Processor: ATmega328P (Old Bootloader)

//=============================================================================

#include <SPI.h>
#include <MFRC522.h>

//=============================================================================

#define RST_PIN         9
#define IRQ_PIN         2
#define SCK_PIN         13
#define MOSI_PIN        11
#define MISO_PIN        12

#define NR_OF_READERS   6

#define SS_1_PIN        10
#define SS_2_PIN        8
#define SS_3_PIN        7
#define SS_4_PIN        6
#define SS_5_PIN        5
#define SS_6_PIN        4

//=============================================================================

byte ssPins[] = {SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN, SS_5_PIN, SS_6_PIN};

MFRC522 mfrc522[NR_OF_READERS];

//=============================================================================

void setup() {

  Serial.begin(115200);
  Serial.println(F("Hello"));

  SPI.begin();

  // IMPORTANT: Disable all Slave-Select lines before initializing readers
  for (uint8_t i=0; i<sizeof(ssPins)/sizeof(ssPins[0]); i++) {
    digitalWrite(ssPins[i], 1);
    pinMode(ssPins[i], OUTPUT );
  }

  // Keep readers in reset
  digitalWrite(RST_PIN, 0);
  pinMode(RST_PIN, OUTPUT );
  delay(1);

}

// ================================================================================

void loop() {
    Serial.println();
    Serial.println(millis());
    
    // Hard-reset all readers
    digitalWrite(RST_PIN, 0);  delay(1);
    digitalWrite(RST_PIN, 1);  delay(50);
    
    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
        mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN);
        Serial.print(F("Reader "));  Serial.print(reader);  Serial.print(F(": "));
        mfrc522[reader].PCD_DumpVersionToSerial();

        // NOTE: When setting gain to 7 (maximum) --> No detection
        // Maximum working gain is 6.
        mfrc522[reader].PCD_SetAntennaGain( 6 << 4 );
        
        if (
            mfrc522[reader].PICC_IsNewCardPresent()  &&
              mfrc522[reader].PICC_ReadCardSerial()) {
            Serial.print(F("\t\t\tCard UID:"));
            dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
            Serial.print(F("\n"));
        }
        
        mfrc522[reader].PCD_SoftPowerDown();
    }
    
}

// ================================================================================

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// ================================================================================
