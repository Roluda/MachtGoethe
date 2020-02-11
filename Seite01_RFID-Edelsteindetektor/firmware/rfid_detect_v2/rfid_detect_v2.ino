//=============================================================================
// Exit the library (Siegel 7)
// RFDI detection
// Detect the presence 5 (or 6) objects with RFID tags at the respective RFID-readers.
//=============================================================================

// Compile for 
//   Board: Ardunino Uno
//   Processor: ATmega328P

//=============================================================================

#include <SPI.h>
#include <MFRC522.h>

//=============================================================================
// RFID config

#define NR_OF_READERS   5

// Individual reader gain setting (1-7). 7 only works with modified hardware.
// Higer values: Increase detection distance, but worse detection at
//               smaller distances.
#define READER_GAIN_0     7  
#define READER_GAIN_1     7
#define READER_GAIN_2     7
#define READER_GAIN_3     7
#define READER_GAIN_4     7

// Required card UIDs (0 = don't care)
#define UID_0   0
#define UID_1   0
#define UID_2   0
#define UID_3   0
#define UID_4   0
#define UID_5   0

//=============================================================================
// Pin mapping

#define HARDWARE_VER  2

#if (HARDWARE_VER == 1)
  #define RST_PIN         9
  #define IRQ_PIN         2
  #define SCK_PIN         13
  #define MOSI_PIN        11
  #define MISO_PIN        12
  
  #define SS_0_PIN        10
  #define SS_1_PIN        8
  #define SS_2_PIN        7
  #define SS_3_PIN        6
  #define SS_4_PIN        5
  #define SS_5_PIN        4
  
  #define LED_0_PIN       A0
  #define LED_1_PIN       A1
  #define LED_2_PIN       A2
  #define LED_3_PIN       A3
  #define LED_4_PIN       A4
  #define LED_5_PIN       A5
  
  #define SUCCESS_OUT_PIN  3
  
#elif (HARDWARE_VER == 2)
  #define RST_PIN         8
  // IRQ line not connected
  #define SCK_PIN         13
  #define MOSI_PIN        11
  #define MISO_PIN        12
  
  #define SS_0_PIN        A5
  #define SS_1_PIN        A4
  #define SS_2_PIN        7
  #define SS_3_PIN        5
  #define SS_4_PIN        4
  #define SS_5_PIN        2
  
  #define LED_0_PIN       9    // PWM T1A
  #define LED_1_PIN       10   // PWM T1B
  #define LED_2_PIN       3    // PWM T2B
  #define LED_3_PIN       6    // PWM T0A
  #define LED_4_PIN       A0
  #define LED_5_PIN       A1
  
  #define SUCCESS_OUT_PIN  A3
  #define SUCCESS_LED_PIN  LED_5_PIN
#endif

//=============================================================================

MFRC522 mfrc522[NR_OF_READERS];
byte ss_pins[] = {SS_0_PIN, SS_1_PIN, SS_2_PIN, SS_3_PIN, SS_4_PIN};
byte led_pins[] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};
byte reader_gains[] = {READER_GAIN_0, READER_GAIN_1, READER_GAIN_2, READER_GAIN_3, READER_GAIN_4};
uint32_t required_uids[] = {UID_0, UID_1, UID_2, UID_3, UID_4};
bool detect[NR_OF_READERS];


//=============================================================================

void setup() {
  digitalWrite( SUCCESS_OUT_PIN, 0 );
  pinMode( SUCCESS_OUT_PIN, OUTPUT );
  #ifdef SUCCESS_LED_PIN
    digitalWrite( SUCCESS_LED_PIN, 0 );
     pinMode( SUCCESS_LED_PIN, OUTPUT );
  #endif

  // All LEDs off
  for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
      digitalWrite(led_pins[i], 0);
      pinMode(led_pins[i], OUTPUT);
  }

  Serial.begin(115200);
  Serial.println(F("Hello"));

  SPI.begin();

  // IMPORTANT: Disable all Slave-Select lines before initializing readers
  for (uint8_t i=0; i<sizeof(ss_pins)/sizeof(ss_pins[0]); i++) {
    digitalWrite(ss_pins[i], 1);
    pinMode(ss_pins[i], OUTPUT );
  }

  // Keep readers in reset
  digitalWrite(RST_PIN, 0);
  pinMode(RST_PIN, OUTPUT );
  delay(1);


}

// ================================================================================

void loop() {

    // Hard-reset all readers
    digitalWrite(RST_PIN, 0);  delay(1);
    digitalWrite(RST_PIN, 1);  delay(50);

   // Init reader
    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
        mfrc522[reader].PCD_Init(ss_pins[reader], RST_PIN);
        Serial.print(F("Reader "));  Serial.print(reader);  Serial.print(F(": "));
        mfrc522[reader].PCD_DumpVersionToSerial();
        mfrc522[reader].PCD_SetAntennaGain( (reader_gains[reader]) << 4 );
    }

    while (1) {
        bool error = 0;
        
        for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
            detect[reader] = 0;

            // Make sure the reader is working
            int ver = read_serial( reader );
            if (ver != 0x92) {  error = 1;  continue;  }
    
            if (mfrc522[reader].PICC_IsNewCardPresent()  &&
                    mfrc522[reader].PICC_ReadCardSerial()) {
                Serial.print( millis() );  Serial.print(F("\t"));
                Serial.print(F("Reader: "));  Serial.print(reader);  Serial.print(F("  "));
                Serial.print(F("UID: "));
                dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
                Serial.print(F("\n"));

                uint32_t uid = *(uint32_t*) &mfrc522[reader].uid.uidByte;
                if ((required_uids[reader] == 0) || 
                      (uid == required_uids[reader])) {
                    detect[reader] = 1;
                }
            }
            
            mfrc522[reader].PCD_SoftPowerDown();
        }
    
        // Switch on leds for detected readers
        for (uint8_t i=0; i<NR_OF_READERS; i++) {
            digitalWrite(led_pins[i], detect[i]);
            pinMode(led_pins[i], OUTPUT);
        }

        // Test for success condition
        bool success = 1;
        for (uint8_t reader = 0; reader<NR_OF_READERS; reader++) {
          success &= detect[reader];
        }
        digitalWrite(SUCCESS_OUT_PIN, success);
        #ifdef SUCCESS_LED_PIN
          digitalWrite(SUCCESS_LED_PIN, success);
        #endif

        // On errors, restart main loop (re-inits all readers)
        if (error) break;
    }
}

// ================================================================================
// Read serial number
int read_serial( int reader ) {
  byte v = mfrc522[reader].PCD_ReadRegister(MFRC522::VersionReg);
  return v;
}

// Read and print serial number
// This is a copy of MFRC522::PCD_DumpVersionToSerial() that additionally 
// returns the read serial code.

int read_print_serial( int reader ) {
  byte v = mfrc522[reader].PCD_ReadRegister(MFRC522::VersionReg);
  Serial.print(F("Firmware Version: 0x"));
  Serial.print(v, HEX);
  switch(v) {
    case 0x88: Serial.println(F(" = (clone)"));  break;
    case 0x90: Serial.println(F(" = v0.0"));     break;
    case 0x91: Serial.println(F(" = v1.0"));     break;
    case 0x92: Serial.println(F(" = v2.0"));     break;
    case 0x12: Serial.println(F(" = counterfeit chip"));     break;
    default:   Serial.println(F(" = (unknown)"));
  }
  if ((v == 0x00) || (v == 0xFF))
    Serial.println(F("WARNING: Communication failure, is the MFRC522 properly connected?"));
  return v;
} 

// ================================================================================

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// ================================================================================
// Test functions

// First reader
void test_rfid(void)
{
    Serial.begin(115200);  Serial.println(F("Hello"));
    SPI.begin();

    while (1) {
        pinMode(ss_pins[0], OUTPUT );
        digitalWrite(ss_pins[0], 1);  
        pinMode(RST_PIN, OUTPUT );
        digitalWrite(RST_PIN, 0);  delay(1);
        digitalWrite(RST_PIN, 1);  delay(50);
    
        mfrc522[0].PCD_Init(ss_pins[0], RST_PIN);
        mfrc522[0].PCD_DumpVersionToSerial();
        mfrc522[0].PCD_SetAntennaGain( 7 << 4 );
        
        while (1) {
            detect[0] = 0;
            int ver = read_serial(0);
            if (ver != 0x92) {  break; }
    
            if (mfrc522[0].PICC_IsNewCardPresent()  &&
                    mfrc522[0].PICC_ReadCardSerial()) {
                Serial.print( millis() ); Serial.print(F("\tCard UID:"));
                dump_byte_array(mfrc522[0].uid.uidByte, mfrc522[0].uid.size);
                Serial.print(F("\n"));
                detect[0] = 1;
            }
            
            mfrc522[0].PCD_SoftPowerDown();
            
            digitalWrite(led_pins[0], detect[0]);
            pinMode(led_pins[0], OUTPUT);
        }
    }
}


// Blink all LEDs with one second period
void test_led(void)
{
  bool on = 1;
  while (1) {
    for (int i=0; i<NR_OF_READERS; i++) {
      digitalWrite( led_pins[i], on );
      pinMode( led_pins[i], OUTPUT );
    }
    on = !on;
    delay(1000);
  }
}

// ================================================================================
