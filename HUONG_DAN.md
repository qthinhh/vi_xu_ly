# Lập trình thanh ghi ATmega328P trên Arduino Uno

Bộ ví dụ này dùng truy cập thanh ghi trực tiếp, không dùng các API `digitalWrite()`, `analogRead()`, `analogWrite()`, `Serial`, `SPI` hoặc `Wire`. Mỗi thư mục là một sketch độc lập, vì vậy hãy mở từng file `.ino` riêng trong Arduino IDE.

## Cấu trúc

| Thư mục | File | Nội dung chính | Kết quả kiểm tra |
|---|---|---|---|
| `GPIO` | `GPIO.ino` | `DDRx`, `PORTx`, `PINx` | Nhấn nút D2 để đổi trạng thái LED D13 |
| `UART` | `UART.ino` | `UBRR0`, `UCSR0A/B/C`, `UDR0` | Echo ký tự trên Serial Monitor |
| `SPI` | `SPI.ino` | `SPCR`, `SPSR`, `SPDR` | Loopback D11-D12; D13 sáng khi nhận đúng |
| `I2C` | `I2C.ino` | `TWBR`, `TWSR`, `TWCR`, `TWDR` | Quét slave; D13 sáng nếu nhận được ACK |
| `ADC` | `ADC.ino` | `ADMUX`, `ADCSRA/B`, `ADC`, `DIDR0` | D13 sáng khi A0 đạt khoảng 2,5 V trở lên |
| `PWM` | `PWM.ino` | `TCCR1A/B`, `OCR1A` | Độ sáng LED D9 tăng giảm |
| `TIMER` | `TIMER.ino` | `TCCR2A/B`, `OCR2A`, `TIMSK2` | LED D13 đổi trạng thái mỗi giây bằng ISR |

## Chuẩn bị và nạp chương trình

1. Mở Arduino IDE.
2. Chọn **Tools → Board → Arduino Uno**, đúng cổng COM/USB.
3. Mở một sketch, ví dụ `GPIO/GPIO.ino`, rồi bấm Verify và Upload.
4. Không đặt nhiều thư mục ví dụ vào cùng một sketch. Mỗi file đều có `setup()` và `loop()` riêng.
5. Tần số giả định là `F_CPU = 16 MHz`, đúng với Arduino Uno thông thường.

## Đấu nối

### GPIO

- LED tích hợp trên Uno dùng D13/PB5.
- Nút nhấn nối giữa D2/PD2 và GND. Code bật pull-up nội nên không cần điện trở ngoài.
- Nút nhấn là active-low: thả nút đọc mức 1, nhấn nút đọc mức 0.

### UART

- D1/PD1 là TX, D0/PD0 là RX và được nối sẵn với USB-UART trên Uno.
- Mở Serial Monitor ở `9600 baud`, cấu hình 8N1.
- Không nên nối thiết bị ngoài vào D0/D1 trong lúc upload vì có thể làm lỗi bootloader.

### SPI

- D10/PB2: SS, D11/PB3: MOSI, D12/PB4: MISO, D13/PB5: SCK.
- Để tự kiểm tra master, nối tạm D11 với D12. Code truyền byte rồi đọc lại byte đó.
- Với slave thật, nối chung GND, MOSI, MISO, SCK; nối D10 tới chân CS/SS của slave. Mỗi slave cần SS riêng.

### I2C/TWI

- A4/PC4 là SDA, A5/PC5 là SCL.
- Cần điện trở kéo lên ngoài, thường 4,7 kOhm từ SDA và SCL lên 5 V. Pull-up nội trong code chỉ là hỗ trợ yếu.
- Code quét các địa chỉ 7-bit từ `0x08` đến `0x77`; nếu có slave trả ACK thì D13 sáng.
- Việc đọc/ghi dữ liệu cụ thể phụ thuộc bản đồ thanh ghi của từng IC. Các hàm mức thấp `twi_start()`, `twi_write_byte()` và `twi_read_byte()` trong file là điểm bắt đầu để xây dựng driver cho IC đó.

### ADC

- Dùng biến trở 10 kOhm: một đầu nối 5 V, một đầu nối GND, chân giữa nối A0/ADC0.
- `ADMUX.REFS0 = 1` chọn AVcc làm điện áp tham chiếu; kết quả 10 bit nằm trong khoảng 0..1023.
- Không đưa điện áp âm hoặc cao hơn AVcc vào chân ADC.

### PWM

- OC1A là D9/PB1. Nối D9 → điện trở 220–330 Ohm → anode LED, cathode LED → GND.
- `OCR1A = 0` tương ứng duty gần 0%, `OCR1A = 255` tương ứng duty gần 100%.
- Ví dụ dùng Timer1 nên không dùng đồng thời thư viện cần Timer1, chẳng hạn một số cấu hình Servo.

### TIMER

- Timer2 chạy CTC với prescaler 64 và `OCR2A = 249`, tạo ngắt 1 ms ở 16 MHz.
- ISR đếm 1000 lần ngắt rồi đảo PB5. Timer2 có thể xung đột với thư viện `tone()` hoặc code khác sử dụng Timer2.
- Luôn giữ ISR ngắn; dữ liệu dùng chung giữa ISR và `loop()` nên khai báo `volatile` và cần chú ý tính nguyên tử khi đọc biến nhiều byte.

## Bảng chân và thanh ghi chính

| Chức năng | Chân ATmega328P | Thanh ghi chính |
|---|---|---|
| GPIO | PORTB/PORTC/PORTD | `DDRx`, `PORTx`, `PINx` |
| UART0 | PD0, PD1 | `UBRR0H/L`, `UCSR0A/B/C`, `UDR0` |
| SPI | PB2..PB5 | `SPCR`, `SPSR`, `SPDR` |
| TWI/I2C | PC4, PC5 | `TWBR`, `TWSR`, `TWCR`, `TWDR` |
| ADC | PC0..PC5 | `ADMUX`, `ADCSRA`, `ADCSRB`, `ADCL/H` |
| PWM Timer1 | PB1/OC1A | `TCCR1A/B`, `OCR1A` |
| Timer2 | nội bộ | `TCCR2A/B`, `OCR2A`, `TIMSK2`, `TCNT2` |

## Lưu ý quan trọng

- Tên bit trong mã (`RXEN0`, `SPIF`, `TWINT`, `ADSC`, ...) được định nghĩa trong header của AVR dành cho ATmega328P.
- Khi đổi clock hoặc baud rate, cần tính lại các giá trị `UBRR0`, `TWBR`, `OCR2A` và prescaler.
- Các phép chờ trong ví dụ là blocking để dễ học. Trong ứng dụng thực tế có thể chuyển sang ngắt hoặc thêm timeout để tránh treo nếu thiết bị ngoại vi bị ngắt kết nối.
