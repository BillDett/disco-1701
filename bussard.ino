/*

Animated bussard lights for Discovery's Enterprise NCC-1701 model from Round 2

Using an ATTiny85 inside each nacelle to 'rotate' the lights instead of the fan blades

Physical Pin arrangement (for clockwise rotation):

        Pin5
         |
   Pin3 -+-  Pin6
         |
        Pin2

Since the ATTiny85 only has 2 default PWM outputs, we're using the "4 PWM" technique as shown in http://www.technoblogy.com/show?LE0

Basic algorithm:
 * Start LEDs 2-4 at half brightness, LED1 at full brightness
 * Ramp LED1 down to half while ramping up LED2 to full
 * Repeat with LED2/LED3, then LED3/LED4, then LED1/LED2, etc...

    0 = full brightness, 255 = fully off

*/

// ATtiny85 outputs
const int LED1 = 0; // OCR0A
const int LED2 = 1; // OCR0B
const int LED3 = 3; // OCR1A
const int LED4 = 4; // OCR1B
volatile uint8_t* Port[] = {&OCR0A, &OCR0B, 0, &OCR1A, &OCR1B};

const int ON = 0;
const int MIDWAY = 128;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  // Configure counter/timer0 for fast PWM on PB0 and PB1
  TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
  TCCR0B = 0<<WGM02 | 3<<CS00; // Optional; already set
  // Configure counter/timer1 for fast PWM on PB4
  TCCR1 = 1<<CTC1 | 1<<PWM1A | 3<<COM1A0 | 7<<CS10;
  GTCCR = 1<<PWM1B | 3<<COM1B0;
  // Interrupts on OC1A match and overflow
  TIMSK = TIMSK | 1<<OCIE1A | 1<<TOIE1;

  // Start off with first LED high, all others low intensity
  OCR0A = ON;
  OCR0B = OCR1A = OCR1B = MIDWAY;
}

ISR(TIMER1_COMPA_vect) {
  if (!bitRead(TIFR,TOV1)) bitSet(PORTB, LED3);
}

ISR(TIMER1_OVF_vect) {
  bitClear(PORTB, LED3);
}

void MoveHighlight(int from, int to) {
  for (int i=ON; i < MIDWAY; i++) {
    *Port[from] = i;      // ramp down
    *Port[to] = MIDWAY - i;  // ramp up
    delay(1);
  }
}

void loop() {

  MoveHighlight(LED1, LED2);
  MoveHighlight(LED2, LED3);
  MoveHighlight(LED3, LED4);
  MoveHighlight(LED4, LED1);

}
