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

#define SW_1_PIN       A0
#define SW_2_PIN       A1

#define SUCCESS_OUT_PIN  A3

//=============================================================================

byte led_pins[] = {LED_0_PIN, LED_1_PIN, LED_2_PIN, LED_3_PIN, SUCCESS_OUT_PIN};

//=============================================================================

void setup() {

  digitalWrite( SUCCESS_OUT_PIN, 0 );
  pinMode( SUCCESS_OUT_PIN, OUTPUT );
  digitalWrite(SW_1_PIN, 0);
  pinMode(SW_1_PIN, OUTPUT);
  pinMode(SW_2_PIN, INPUT_PULLUP);

  // All LEDs off
  for (uint8_t i=0; i<sizeof(led_pins)/sizeof(led_pins[0]); i++) {
      digitalWrite(led_pins[i], 0);
      pinMode(led_pins[i], OUTPUT);
  }

  pwm_init();
}

// ================================================================================

void loop() {
  if (digitalRead(SW_2_PIN) == 0)  {
    //test_single();
    //test_blink();
    //test_fade();
    test_color();
  }
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

struct RGBA {
  uint8_t red, green, blue, alpha;
  RGBA(uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t a=0) : red(r), green(g), blue(b), alpha(a) {};
  
  
};


RGBA Yellow(255,255,0,255);


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

#define FADE_DELAY  5
#define FADE_MAX    50



void test_color(void)
{
    for (int16_t j=1; j<FADE_MAX; j++) {
      set_color( 255,255,0,j);
      delay(FADE_DELAY);
    }
    for (int16_t j=FADE_MAX; j>=0; j--) {
      set_color( 255,255,0,j);
      delay(FADE_DELAY);
    }
}

// ================================================================================

void pwm_init(void)
{
  // Increase PWM frequency
  #ifdef TCCR0B
    // NOTE: Do not change Timer 0 settings, since it controls some Arduino internals.
    //TCCR0B = (TCCR0B & 0b11111000) | 1;  // Timer 0 clk div 1
  #endif
  #ifdef TCCR1B
      TCCR1B = (TCCR1B & 0b11111000) | 1;  // Timer 1 clk div 1
  #endif
  #ifdef TCCR2B
      TCCR2B = (TCCR2B & 0b11111000) | 1;  // Timer 2 clk div 1
  #endif
  #ifdef TCCR3B
    TCCR3B = (TCCR3B & 0b11111000) | 1;  // Timer 3 clk div 1
  #endif
  #ifdef TCCR4B
    TCCR4B = (TCCR4B & 0b11110000) | 1;  // Timer 4 clk div 1
  #endif  
}

// ================================================================================
