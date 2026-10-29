// I2C.ino - TWI/I2C master bang thanh ghi cho Arduino Uno / ATmega328P
// SDA: A4 (PC4), SCL: A5 (PC5), toc do 100 kHz.
// Vi du nay quet dia chi slave 0x08..0x77; D13 sang neu tim thay ACK.
// Can dien tro keo len ngoai khoang 4.7 kOhm tu SDA/SCL len 5 V.

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>
#include <util/twi.h>

static void twi_init(void)
{
  // TWI dung chan PC4/PC5. Pull-up ngoai van duoc khuyen nghi.
  DDRC &= ~(_BV(DDC4) | _BV(DDC5));
  PORTC |= _BV(PORTC4) | _BV(PORTC5);

  // Prescaler = 1; SCL = F_CPU / (16 + 2 * TWBR * prescaler).
  TWSR = 0;
  TWBR = (uint8_t)(((F_CPU / 100000UL) - 16UL) / 2UL);
  TWCR = _BV(TWEN);
}

static bool twi_start(uint8_t address_rw)
{
  uint8_t status;

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
  while ((TWCR & _BV(TWINT)) == 0) {
    // Cho phat START.
  }

  status = TWSR & 0xF8;
  if (status != TW_START && status != TW_REP_START) {
    return false;
  }

  TWDR = address_rw;
  TWCR = _BV(TWINT) | _BV(TWEN);
  while ((TWCR & _BV(TWINT)) == 0) {
    // Cho truyen SLA+R/W.
  }

  status = TWSR & 0xF8;
  return (status == TW_MT_SLA_ACK) || (status == TW_MR_SLA_ACK);
}

static void twi_stop(void)
{
  TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
  // STOP tu dong hoan tat; khong can cho TWINT.
}

static bool __attribute__((unused)) twi_write_byte(uint8_t data)
{
  TWDR = data;
  TWCR = _BV(TWINT) | _BV(TWEN);
  while ((TWCR & _BV(TWINT)) == 0) {
    // Cho truyen data.
  }
  return (TWSR & 0xF8) == TW_MT_DATA_ACK;
}

static uint8_t __attribute__((unused)) twi_read_byte(bool acknowledge)
{
  if (acknowledge) {
    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
  } else {
    TWCR = _BV(TWINT) | _BV(TWEN);
  }

  while ((TWCR & _BV(TWINT)) == 0) {
    // Cho nhan data.
  }
  return TWDR;
}

static bool i2c_probe(uint8_t address)
{
  bool acknowledged = twi_start((uint8_t)(address << 1));
  twi_stop();
  return acknowledged;
}

static bool i2c_device_found(void)
{
  uint8_t address;

  // Bo qua cac dia chi reserved theo khung dia chi I2C 7 bit.
  for (address = 0x08; address <= 0x77; address++) {
    if (i2c_probe(address)) {
      return true;
    }
  }
  return false;
}

void setup()
{
  DDRB |= _BV(DDB5); // LED D13
  PORTB &= ~_BV(PORTB5);
  twi_init();
}

void loop()
{
  if (i2c_device_found()) {
    PORTB |= _BV(PORTB5);
  } else {
    PORTB &= ~_BV(PORTB5);
  }
  _delay_ms(1000);
}
