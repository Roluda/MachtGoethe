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
#include <EEPROM.h>
#include <MFRC522.h>

//=============================================================================
// RFID config

// Reader antenna gain: 1-7
//   Higher values increases detection distance, but may be unreliably for short distances 
//   7 only works with modified reader hardware.
#define READER_GAIN     5

// Required card UID (0 = don't care or use UID stored in EEPROM)
#define REQUIRED_UID   0

//=============================================================================
// Pin mapping

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
  
  #define SW_4_PIN       A0  // Switch is connected between SW_4_PIN and SW_5_PIN
  #define SW_5_PIN       A1
  
  #define SUCCESS_OUT_PIN  A3

//=============================================================================

byte led_pins[] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN};

MFRC522 mfrc522;

struct {
  uint32_t stored_uid;
  bool     valid;
} eep = {0};

//=============================================================================

void setup() {
  digitalWrite( SW_4_PIN, 0 );
  pinMode( SW_4_PIN, OUTPUT );
  pinMode( SW_5_PIN, INPUT_PULLUP );
  
  digitalWrite( SUCCESS_OUT_PIN, 0 );
  pinMode( SUCCESS_OUT_PIN, OUTPUT );
  
  // All LEDs off
  for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
      digitalWrite(led_pins[i], 0);
      pinMode(led_pins[i], OUTPUT);
  }

  pwm_init();
  
  Serial.begin(115200);
  Serial.println(F("Hello"));

  SPI.begin();

  // Init Slave-select
  digitalWrite(SS_0_PIN, 1);
  pinMode(SS_0_PIN, OUTPUT );

  // Keep reader in reset
  digitalWrite(RST_PIN, 0);
  pinMode(RST_PIN, OUTPUT );
  delay(1);

  // Clear EEPROM
  if (digitalRead(SW_5_PIN) == 0) {
    delay(1000);
    if (digitalRead(SW_5_PIN) == 0) {
        Serial.println("Clearing EEPROM...");
        for (int i = 0; i < 512; i++)
            EEPROM.write(i, 0);
        Serial.println("Ok");
    }
    while (digitalRead(SW_5_PIN) == 0)  // Wait for realease
      delay(1);
  }

  // Load EEPROM data
  EEPROM.get(0, eep);
  if (!eep.valid) eep.stored_uid = 0;
  Serial.print("Stored UID:  ");
  dump_byte_array((byte*) &eep.stored_uid, 4);
  Serial.print("\n");
}

// ================================================================================

void loop() {
    // Hard-reset reader
    digitalWrite(RST_PIN, 0);  delay(1);
    digitalWrite(RST_PIN, 1);  delay(50);

   // Init reader
    mfrc522.PCD_Init(SS_0_PIN, RST_PIN);
    Serial.print("Reader: ");
    mfrc522.PCD_DumpVersionToSerial();

    // NOTE: Gain setting 7 only works with modified hardware and may be not reliably at short distances.    
    mfrc522.PCD_SetAntennaGain( (READER_GAIN) << 4 );
    
    while (1) {
        bool error = 0;
        bool detect = 0;

        if (digitalRead(SW_5_PIN) == 0) {
            delay(1000);
            if (digitalRead(SW_5_PIN) == 0)
                learn_uid();
        }

        // Make sure the reader is working
        int ver = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
        if (ver != 0x92) {
            error = 1;
        }
    
        if (mfrc522.PICC_IsNewCardPresent()  &&
              mfrc522.PICC_ReadCardSerial()) {
            Serial.print( millis() );  Serial.print("\t");
            Serial.print("UID: ");
            dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
            Serial.print("\n");

            uint32_t uid = *(uint32_t*) &mfrc522.uid.uidByte;
            if (REQUIRED_UID != 0) {
                if (uid == REQUIRED_UID) {
                    Serial.println("Tag matches REQUIRED_UID");
                    detect = 1;
                }
            }
            else if (eep.valid && (eep.stored_uid != 0)) {
                if (uid == eep.stored_uid) {
                    Serial.println("Tag matches learned UID");
                    detect = 1;
                }
            }
            else if (uid != 0) {
                Serial.println("All tags accepted");
                detect = 1;
            }
        }
        else {
            //Serial.print( millis() );  Serial.print("\t");
            //Serial.print("No tag");
            //Serial.print("\n");
        }

        mfrc522.PCD_SoftPowerDown();

        // Debounce success out
        static uint8_t success = 0;
        if (detect) success = 4;
        else if (success > 0) success--;
        
        // Test for success condition
        digitalWrite(SUCCESS_OUT_PIN, success);
        if (success)  led_animation_step();
        else          led_animation_off();

        // On errors, restart main loop (re-inits all readers)
        if (error) break;
    }
}

// ================================================================================
// Read serial number
int read_serial( int reader ) {
  byte v = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
  return v;
}

// Read and print serial number
// This is a copy of MFRC522::PCD_DumpVersionToSerial() that additionally 
// returns the read serial code.

int read_print_serial( int reader ) {
  byte v = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
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

void learn_uid(void)
{
    // Make sure the reader is working
    int ver = mfrc522.PCD_ReadRegister(MFRC522::VersionReg);
    if (ver != 0x92) return;

    Serial.println("BEGIN LEARNING UID...");

    uint8_t blink_state = 0;
    uint32_t t0 = millis(); 
    while (millis() - t0 < 20000) {
        blink_state ^= 1;
        digitalWrite( SUCCESS_OUT_PIN, blink_state );
  
        if (mfrc522.PICC_IsNewCardPresent()  &&
              mfrc522.PICC_ReadCardSerial()) {
            Serial.print( millis() );  Serial.print("\t");
            Serial.print("UID: ");
            dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
            Serial.print("\n");
    
            uint32_t uid = *(uint32_t*) &mfrc522.uid.uidByte;
            if (uid != 0) {
                eep.stored_uid = uid;
                eep.valid = 1;
                EEPROM.put(0, eep);
                Serial.print("STORED UID: ");
                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.print("\n");
                return;
            }
        }
    }
    Serial.println("TIMEOUT, STOP LEARNING");
}

// ================================================================================
// Additive color type (RGB + Alpha)

struct RGBA {
  uint8_t red, green, blue, alpha;
  RGBA(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=255) : red(r), green(g), blue(b), alpha(a) {};
  RGBA operator * (const uint8_t s) {  // scalar multiplication: dim intensity to alpha *= s/256
    return RGBA( red, green, blue, ((uint16_t)alpha * s + 255) >> 8);
  }
};

// Example colors
const RGBA 
  Red    (255, 0,   0,   255),
  Green  (0,   255, 0,   255),
  Blue   (0,   0,   255, 255),
  Yellow (255, 255, 0,   180),
  Cyan   (0,   255, 255, 180),
  Magenta(255, 0  , 255, 180);

// Stone colors
const RGBA 
  Stone_Red   ( 255, 0,   0,   120 ),
  Stone_Green ( 0,   255, 0,   220 ),
  Stone_Blue  ( 0,   100, 255, 255 ),
  Stone_Pink  ( 255,  86, 164, 120 ),
  Stone_Yellow( 255, 255, 0,   120 );

// ================================================================================
// Set LED to RGBA color

void set_color( uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  r = ((uint16_t)r * a + 255)>>8;
  g = ((uint16_t)g * a + 255)>>8;
  b = ((uint16_t)b * a + 255)>>8;
  analogWrite( led_pins[0], r );
  analogWrite( led_pins[1], g );
  analogWrite( led_pins[2], b );
}

void set_color( struct RGBA rgba )
{
  set_color( rgba.red, rgba.green, rgba.blue, rgba.alpha );
}

          
// ================================================================================
// LED animation on success

RGBA colors[] = { Stone_Red, Stone_Green, Stone_Blue, Stone_Pink, Stone_Yellow };
#define N_COLORS (sizeof(colors)/sizeof(colors[0]))

#define FADE_DELAY  0
#define FADE_MAX    150
#define FADE_STEP   15

void led_animation_off(void)
{
  for (uint8_t i=0; i<3; i++) {
    digitalWrite(led_pins[i], 0);
  }
}

void led_animation_step(void)
{
  static struct {
    uint32_t last_time;
    int16_t fade;
    uint8_t color, dir;
  } state;

  if ((FADE_DELAY > 0) && (millis() - state.last_time < FADE_DELAY))
      return;
  state.last_time += FADE_DELAY;

  if (state.dir == 1)
  {
    if (state.fade < FADE_MAX) {
      state.fade += FADE_STEP;
    }
    else {
      state.dir = 0;
    }
  }
  else { // state.dir == 0
    if (state.fade > 0) {
      state.fade -= FADE_STEP;
    }
    else {
      state.dir = 1;
      if (++state.color >= N_COLORS)  state.color = 0;
    }
  }
  if (state.fade > FADE_MAX)  state.fade = FADE_MAX;
  if (state.fade < 0)  state.fade = 0;
  set_color( colors[state.color] * state.fade );
}

// ================================================================================
// Test functions

void test_rfid(void)
{
    Serial.begin(115200);  Serial.println(F("Hello"));
    SPI.begin();

    while (1) {
        pinMode(SS_0_PIN, OUTPUT );
        digitalWrite(SS_0_PIN, 1);  
        pinMode(RST_PIN, OUTPUT );
        digitalWrite(RST_PIN, 0);  delay(1);
        digitalWrite(RST_PIN, 1);  delay(50);
    
        mfrc522.PCD_Init(SS_0_PIN, RST_PIN);
        mfrc522.PCD_DumpVersionToSerial();
        mfrc522.PCD_SetAntennaGain( 7 << 4 );
        
        while (1) {
            bool detect = 0;
            int ver = read_serial(0);
            if (ver != 0x92) {  break; }
    
            if (mfrc522.PICC_IsNewCardPresent()  &&
                    mfrc522.PICC_ReadCardSerial()) {
                Serial.print( millis() ); Serial.print("\tCard UID:");
                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.print("\n");
                detect = 1;
            }
            
            digitalWrite(led_pins[0], detect);
            pinMode(led_pins[0], OUTPUT);
        }
    }
}


// ================================================================================

void pwm_init(void)
{
  // Increase PWM frequency
  #ifdef TCCR0B
    // NOTE: Do not change Timer 0 settings, since it controls some Arduino internals.
    //TCCR0B = TCCR0B & 0b11111000 | 1;  // Timer 0 clk div 1
  #endif
  #ifdef TCCR1B
      TCCR1B = TCCR1B & 0b11111000 | 1;  // Timer 1 clk div 1
  #endif
  #ifdef TCCR2B
      TCCR2B = TCCR2B & 0b11111000 | 1;  // Timer 2 clk div 1
  #endif
  #ifdef TCCR3B
    TCCR3B = TCCR3B & 0b11111000 | 1;  // Timer 3 clk div 1
  #endif
  #ifdef TCCR4B
    TCCR4B = TCCR4B & 0b11110000 | 1;  // Timer 4 clk div 1
  #endif  
}

// ================================================================================
