// ADC.ino - ADC bang thanh ghi cho Arduino Uno / ATmega328P
// A0 (ADC0/PC0): dien ap dau vao 0..Vcc.
// D13 sang khi gia tri ADC >= 512 (xap xi 2.5 V neu Vref = AVcc = 5 V).

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

static void adc_init(void)
{
  // Vref = AVcc, ket qua can phai, chon kenh ADC0 ban dau.
  ADMUX = _BV(REFS0);

  // Bat ADC, prescaler 128 => clock ADC = 125 kHz voi F_CPU = 16 MHz.
  ADCSRA = _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  ADCSRB = 0;       // free-running mode khong dung trong vi du nay
  DIDR0 = _BV(ADC0D); // tat input digital o chan ADC0
}

static uint16_t adc_read(uint8_t channel)
{
  // ADC0..ADC5 tuong ung A0..A5.
  ADMUX = _BV(REFS0) | (channel & 0x07);
  ADCSRA |= _BV(ADSC); // bat dau chuyen doi

  while (ADCSRA & _BV(ADSC)) {
    // Cho chuyen doi xong.
  }
  return ADC;
}

void setup()
{
  DDRB |= _BV(DDB5);
  PORTB &= ~_BV(PORTB5);
  adc_init();
}

void loop()
{
  uint16_t value = adc_read(0);

  if (value >= 512) {
    PORTB |= _BV(PORTB5);
  } else {
    PORTB &= ~_BV(PORTB5);
  }
  _delay_ms(20);
}
