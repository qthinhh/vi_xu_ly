// UART.ino - USART0 bang thanh ghi cho Arduino Uno / ATmega328P
// TX: D1 (PD1), RX: D0 (PD0), 9600 baud, 8 data bits, no parity, 1 stop bit.

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <stdint.h>

#define UART_BAUD 9600UL

static void uart_init(uint32_t baud)
{
  // Chon normal speed: baud = F_CPU / (16 * (UBRR0 + 1)).
  uint16_t ubrr = (uint16_t)((F_CPU / (16UL * baud)) - 1UL);

  UCSR0A = 0;
  UBRR0H = (uint8_t)(ubrr >> 8);
  UBRR0L = (uint8_t)ubrr;

  // Cho phep thu va phat; khong bat ngat de giu vi du don gian.
  UCSR0B = _BV(RXEN0) | _BV(TXEN0);

  // 8 bit data, 1 stop bit, no parity (8N1).
  UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

static void uart_write_char(char c)
{
  while ((UCSR0A & _BV(UDRE0)) == 0) {
    // Cho thanh ghi truyen trong.
  }
  UDR0 = (uint8_t)c;
}

static void uart_write_text(const char *text)
{
  while (*text != '\0') {
    uart_write_char(*text++);
  }
}

void setup()
{
  uart_init(UART_BAUD);
  uart_write_text("ATmega328P UART register demo\r\n");
  uart_write_text("Nhap ky tu de echo: ");
}

void loop()
{
  if (UCSR0A & _BV(RXC0)) {
    char received = (char)UDR0;
    uart_write_char(received);
    if (received == '\r') {
      uart_write_char('\n');
    }
  }
}
