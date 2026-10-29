// SPI.ino - SPI master bang thanh ghi cho Arduino Uno / ATmega328P
// D10 (PB2): SS, D11 (PB3): MOSI, D12 (PB4): MISO, D13 (PB5): SCK.
// De test loopback, noi day D11 (MOSI) voi D12 (MISO).

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

static void spi_init_master(void)
{
  // SS, MOSI, SCK la output; MISO la input.
  DDRB |= _BV(DDB2) | _BV(DDB3) | _BV(DDB5);
  DDRB &= ~_BV(DDB4);

  // SS o muc cao khi chua giao tiep.
  PORTB |= _BV(PORTB2);

  // SPI enable, master, clock = F_CPU / 16, mode 0 (CPOL=0, CPHA=0).
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(SPR0);
  SPSR = 0; // SPI2X = 0
}

static uint8_t spi_transfer(uint8_t data)
{
  SPDR = data;
  while ((SPSR & _BV(SPIF)) == 0) {
    // Cho truyen xong.
  }
  return SPDR;
}

void setup()
{
  spi_init_master();
}

void loop()
{
  static uint8_t value = 0xA5;
  uint8_t received;

  PORTB &= ~_BV(PORTB2); // chon slave (SS = 0)
  received = spi_transfer(value);
  PORTB |= _BV(PORTB2);  // bo chon slave

  // Neu loopback D11-D12 dung, D13 sang trong 100 ms.
  // Luu y D13 cung la chan SCK va se nhap nhay trong luc truyen.
  if (received == value) {
    PORTB |= _BV(PORTB5);
    _delay_ms(100);
    PORTB &= ~_BV(PORTB5);
  }

  value++;
  _delay_ms(400);
}
