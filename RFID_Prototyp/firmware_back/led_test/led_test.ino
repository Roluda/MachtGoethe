//=============================================================================
// Exit the library
// Last page RFID detector
// LED test program
//=============================================================================

// Compile for 
//   Board: Ardunino Uno
//   Processor: ATmega328P

#define LED_0_PIN        9    // PWM T1A
#define LED_1_PIN       10    // PWM T1B
#define LED_2_PIN        3    // PWM T2B
#define LED_3_PIN        6    // PWM T0A
#define LED_4_PIN       A0
#define LED_5_PIN       A1

#define SUCCESS_OUT_PIN  A3

//=============================================================================

byte led_pins[] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN, LED_5_PIN, SUCCESS_OUT_PIN};

//=============================================================================

void setup() {

  digitalWrite( SUCCESS_OUT_PIN, 0 );
  pinMode( SUCCESS_OUT_PIN, OUTPUT );

  // All LEDs off
  for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
      digitalWrite(led_pins[i], 0);
      pinMode(led_pins[i], OUTPUT);
  }
}

// ================================================================================

void loop() {
  //test_single();
  //test_blink();
  test_fade();
}

// ================================================================================

#define SINGLE_LED  1

void test_single() {
  bool on = 1;
  while (1) {
    digitalWrite( led_pins[SINGLE_LED], on );
    pinMode( led_pins[SINGLE_LED], OUTPUT );
    on = !on;
    delay(1000);
  }
}


// ================================================================================

void test_blink(void)
{
  while (1) {
    bool on = 1;
    while (1) {
    for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
        digitalWrite( led_pins[i], on );
        pinMode( led_pins[i], OUTPUT );
      }
      on = !on;
      delay(1000);
    }
  }
}

// ================================================================================

#define FADE_DELAY  5
#define FADE_MAX    50

void test_fade(void)
{
  for (uint8_t i=0; i<4; i++) {
    for (int16_t j=1; j<FADE_MAX; j++) {
      analogWrite(led_pins[i], j);
      delay(FADE_DELAY);
    }
    for (int16_t j=FADE_MAX; j>=0; j--) {
      analogWrite(led_pins[i], j);
      delay(FADE_DELAY);
    }
  }
}


// ================================================================================
