// TIMER.ino - Timer2 CTC va ngat bang thanh ghi cho Arduino Uno / ATmega328P
// Timer2 tao ngat moi 1 ms; D13 (PB5) dao trang thai moi 1 giay.

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

volatile uint16_t timer2_milliseconds = 0;

ISR(TIMER2_COMPA_vect)
{
  timer2_milliseconds++;
  if (timer2_milliseconds >= 1000) {
    timer2_milliseconds = 0;
    PORTB ^= _BV(PORTB5);
  }
}

static void timer2_init_ctc_1ms(void)
{
  cli();

  // Timer2 CTC, TOP = OCR2A.
  TCCR2A = _BV(WGM21);
  TCCR2B = 0;
  TCNT2 = 0;
  OCR2A = 249; // 16 MHz / 64 / (249 + 1) = 1 kHz = 1 ms

  TCCR2B = _BV(CS22); // prescaler 64
  TIMSK2 = _BV(OCIE2A);

  sei();
}

void setup()
{
  DDRB |= _BV(DDB5);
  PORTB &= ~_BV(PORTB5);
  timer2_init_ctc_1ms();
}

void loop()
{
  // Moi cong viec duoc thuc hien trong ISR; loop de trong.
}
