
//================================================================
// Exit the library (4)
// Touch detection
// Detect if a human or a chain of humans connect two electrical contact plates.

// One contact (SIGNAL_OUT) is switched alternatively between 0 and Vcc.
// The other contact (SIGNAL_IN) is terminated to Gnd and Vcc with 100M resistors, giving
// a DC bias of Vcc/2. We measure the analog input voltage on SIGNAL_IN:
//   SIGNAL_OUT = 0    --> Voff
//   SIGNAL_OUT = Vcc  --> Von

// When the contacts are closed by a resistance chain up to some 100M:
//   Voff falls, Von rises
// When only a single plate is touched, or connected to a fixed potential:
//  Both Voff and Von should be equal.

// Contact detection condition:
//      Voff < Vcc/2 - THRESHOLD  and
//      Von  > Vcc/2 + THRESHOLD

//================================================================

#define PIN_SIGNAL_OUT  A1
#define PIN_SIGNAL_IN   A0
#define PIN_LED_DETECT  3
#define PIN_GND_DETECT  8

#define PIN_PREVIOUS_SOLVED_IN 6
#define PIN_LOCK_SIGNAL_OUT 4

// Box unlock output
#define PIN_BOX_SIGNAL_OUT  2
#define BOX_T_ACTIVE        5000   // (ms) Sustain active time
#define BOX_POL_ACTIVE      HIGH   // Active out polarity

// Measurement signal
#define SIGNAL_BIAS      512   // ADC bias level
#define SIGNAL_THRES     100   // Detection threshold
#define SIGNAL_DEB_MS    500  // Debounce time for trigger


//================================================================

void setup() {
  Serial.begin(19200);

  // Init pins
  digitalWrite( PIN_SIGNAL_OUT, 0 );
  pinMode( PIN_SIGNAL_OUT, OUTPUT );

  #ifdef PIN_LED_DETECT
    digitalWrite( PIN_LED_DETECT, 0 );
    pinMode( PIN_LED_DETECT, OUTPUT );
  #endif
  #ifdef PIN_GND_DETECT
    digitalWrite( PIN_GND_DETECT, 0 );
    pinMode( PIN_GND_DETECT, OUTPUT );
  #endif

  digitalWrite( PIN_BOX_SIGNAL_OUT, !BOX_POL_ACTIVE);
  pinMode( PIN_BOX_SIGNAL_OUT, OUTPUT);

  digitalWrite( PIN_LOCK_SIGNAL_OUT, 0);
  pinMode(PIN_LOCK_SIGNAL_OUT, OUTPUT);

  pinMode(PIN_PREVIOUS_SOLVED_IN, INPUT_PULLUP);

  analogRead( PIN_SIGNAL_IN );  // To init the pin mode to analog

  Serial.println("Hello!");

}

//================================================================
// Analog input acquisition
// To suppress ripple noise, we average 800 ADC conversions in 
// intervals of 125us. The total acquisition time of 100ms is an
// integer multiple of both 50Hz and 60Hz mains frequency period
// and should therefore result in an optimum ripple suppression
// in both cases.

#if 1
// Uses Timer 2 for 1ms-clock generation
uint16_t acq( int chan )
{
  // Timer 2 to 125us interval
  TCCR2A = 0b00000010;  // CTC mode
  TCCR2B = 0b00000010;  // PS = 1:8
  OCR2A = 250-1;        // interval
  TIFR2 |= (1 << OCF2A);   // Clear interrupt flag

  // Increase ADC speed (reduces resolution)
  ADCSRA = ADCSRA & ~0b111 | 0b101;  // PS = 1:64 --> 52us conversion interval
  
  const uint16_t n = 800;
  uint32_t acc = 0;
  
  for (uint16_t i = 0; i<n; i++) {
    while (!(TIFR2 & (1 << OCF2A)))  // wait for interrupt flag
      ;  // wait
    TIFR2 |= (1 << OCF2A);  // clear interrupt flag
    acc += analogRead( chan );
    digitalWrite(A5, 1);
    digitalWrite(A5, 0);
  }
  return acc/n;
}

//------------------
#else  
// Less precise version, but does not mess with Timer 2.
// Use this if you need the Timer 2 PWM outputs.

uint16_t acq( int chan )
{
  const uint16_t n = 800;
  uint32_t acc = 0;
  uint32_t t0 = micros();
  
  for (uint16_t i = 0; i<n; i++) {
    t0 += 125;
    while (micros() < t0)  // wait for interrupt flag
      ;  // wait
    acc += analogRead( chan );
  }
  return acc/n;
}
#endif

//================================================================

void loop() {
  bool trigger = 0;
  bool detect = 0;
  uint32_t t_detect = 0;

  while (1) {
    // Measure on and off signals
    digitalWrite( PIN_SIGNAL_OUT, 1 );
    delay( 200 );  // signal rise time
    uint16_t v_on = acq( PIN_SIGNAL_IN );
    
    digitalWrite( PIN_SIGNAL_OUT, 0 );
    delay( 200 );
    uint16_t v_off = acq( PIN_SIGNAL_IN );
  
    // Input signal detection
    if ((v_on  >= SIGNAL_BIAS + SIGNAL_THRES) &&
        (v_off <= SIGNAL_BIAS - SIGNAL_THRES)) {
      if (!detect) {
        detect = 1;
        t_detect = millis();
      }
    }
    else {
      detect = 0;
    }

    // LED signal
    digitalWrite( PIN_LED_DETECT, detect );
    
    // PUT further actions here
    // XXXX
  
    // Trigger debounce
    if (detect) {
      if (!trigger  &&
          (millis() - t_detect > SIGNAL_DEB_MS)) {
        trigger = 1;
      }
    }
    else {
      trigger = 0;
    }

    // serial output to plot
    Serial.print(v_on);  
    Serial.print("  ");
    Serial.print(v_off);
    Serial.print("  ");
    Serial.print(detect*100);
    Serial.print("  ");
    Serial.print(trigger*100);
    Serial.println();

    OpenBox(trigger);
    if(trigger){      
      if(digitalRead(PIN_PREVIOUS_SOLVED_IN)==LOW){
        digitalWrite(PIN_LOCK_SIGNAL_OUT, HIGH);
      }
    }
   
  }  
}


//=========================================================================================


void OpenBox(bool trig) {
  static uint32_t t_last_trig = 0;
  static bool active = 0;
  uint32_t t = millis();
  if (trig) {
    t_last_trig = t;
    active = 1;
  }
  else if (active && (t - t_last_trig > BOX_T_ACTIVE)) {
    active = 0;
  }
  if (active) {
    digitalWrite(PIN_BOX_SIGNAL_OUT, BOX_POL_ACTIVE);
  }
  else {
    digitalWrite(PIN_BOX_SIGNAL_OUT, !BOX_POL_ACTIVE);
  }
}

//=========================================================================================
