// GPIO.ino - GPIO bang thanh ghi cho Arduino Uno / ATmega328P
// D13 (PB5): LED tren board
// D2  (PD2): nut nhan noi xuong GND, dung pull-up noi

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

void setup()
{
  // PB5 la output, muc thap luc khoi dong de LED tat.
  DDRB |= _BV(DDB5);
  PORTB &= ~_BV(PORTB5);

  // PD2 la input va bat dien tro keo len noi.
  DDRD &= ~_BV(DDD2);
  PORTD |= _BV(PORTD2);
}

void loop()
{
  // Nut nhan active-low: nhan = PIND2 doc duoc 0.
  if ((PIND & _BV(PIND2)) == 0) {
    PORTB ^= _BV(PORTB5); // dao trang thai LED
    _delay_ms(100);       // debounce don gian
  } else {
    PORTB &= ~_BV(PORTB5);
  }
}
