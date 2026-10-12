#include <avr/io.h>
#include <avr/interrupt.h>

#define LEDPIN 13

volatile byte seconds = 0;

volatile boolean pausePhase = false;

void setup() {
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);
  Serial.begin(9600);
  Serial.println("Start: 5s blink -> 5s off -> repeat");

  cli();
  TCCR1A = 0; //сброс регистров 
  TCCR1B = 0;
  TCNT1  = 0; //таймер
  OCR1A = 15624; //регистр совпадения для канала .тики 
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  sei();
}

void loop() {
}
ISR(TIMER1_COMPA_vect) {
  seconds++;

  if (seconds <= 5) {
    if (!pausePhase) {
      digitalWrite(LEDPIN, !digitalRead(LEDPIN));
    }
  }
  else if (seconds <= 10) {
    if (!pausePhase) {
      digitalWrite(LEDPIN, LOW);
      pausePhase = true;
    }
  }
  else {
    seconds = 0;
    pausePhase = false;
    digitalWrite(LEDPIN, HIGH);
  }
}