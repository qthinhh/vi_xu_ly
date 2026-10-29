// PWM.ino - PWM bang Timer1/OCR1A cho Arduino Uno / ATmega328P
// OC1A = D9 (PB1). Noi LED qua dien tro 220..330 ohm xuong GND.
// Fast PWM 8-bit, non-inverting, tan so xap xi 7.812 kHz.

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

static void pwm_init(void)
{
  DDRB |= _BV(DDB1); // OC1A/D9 la output

  // Fast PWM 8-bit (mode 5: WGM12:0 = 101), non-inverting OC1A.
  TCCR1A = _BV(COM1A1) | _BV(WGM10);
  TCCR1B = _BV(WGM12) | _BV(CS11); // prescaler = 8
  OCR1A = 0;
}

void setup()
{
  pwm_init();
}

void loop()
{
  uint16_t duty;

  for (duty = 0; duty < 256; duty++) {
    OCR1A = (uint8_t)duty;
    _delay_ms(5);
  }

  for (duty = 255; duty > 0; duty--) {
    OCR1A = (uint8_t)duty;
    _delay_ms(5);
  }
  OCR1A = 0;
}
