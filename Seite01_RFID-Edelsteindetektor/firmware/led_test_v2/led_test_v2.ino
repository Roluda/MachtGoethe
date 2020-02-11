//=============================================================================
// Exit the library (Siegel 7)
// LED test program
//=============================================================================

// Compile for 
//   Board: Ardunino Uno
//   Processor: ATmega328P

//=============================================================================

#define HARDWARE_VER  2

#if (HARDWARE_VER == 1)  
  #define LED_0_PIN       A0
  #define LED_1_PIN       A1
  #define LED_2_PIN       A2
  #define LED_3_PIN       A3
  #define LED_4_PIN       A4
  #define LED_5_PIN       A5
  #define SUCCESS_OUT_PIN  3
  
#elif (HARDWARE_VER == 2)
  #define LED_0_PIN       9    // PWM T1A
  #define LED_1_PIN       10   // PWM T1B
  #define LED_2_PIN       3    // PWM T2B
  #define LED_3_PIN       6    // PWM T0A
  #define LED_4_PIN       A0
  #define LED_5_PIN       A1
  #define SUCCESS_OUT_PIN  A3
#endif

//=============================================================================

byte led_pins[] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN, LED_5_PIN, SUCCESS_OUT_PIN};

//=============================================================================

void setup() {

  // All LEDs off
  for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
      digitalWrite(led_pins[i], 0);
      pinMode(led_pins[i], OUTPUT);
  }
}

// ================================================================================

void loop() {
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

// ================================================================================
