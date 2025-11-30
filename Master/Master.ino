#include <LiquidCrystal_I2C.h>

#define SS_PIN   10
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN  13
#define HEATER_LED 7

#define BTN_UP 2
#define BTN_DOWN 3

volatile int tempThreshold = 25;
volatile float temperature = 0, humidity = 0;

// 16x2 LCD at address 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {

  
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  DDRB &= ~(1 << PB4);
  PORTB |= (1 << PB2);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  DDRD |= (1 << PD7);

  // Buttons
  DDRD &= ~((1 << PD2) | (1 << PD3));
  PORTD |= (1 << PD2) | (1 << PD3);
  EICRA |= (1 << ISC01) | (1 << ISC00);
  EICRA |= (1 << ISC11) | (1 << ISC10);
  EIMSK |= (1 << INT0) | (1 << INT1);

  lcd.init();
  lcd.backlight();
  lcd.clear();
}

void loop() {
  byte humCommand = spiTransfer(0x02); // send command
byte humData = spiTransfer(0x00);    // read response

byte tempCommand = spiTransfer(0x01); // send command
byte tempData = spiTransfer(0x00);    // read response

  temperature = tempData;
  humidity = humData;

  // Heater control
  if (temperature < tempThreshold)
    PORTD |= (1 << PD7); 
  else
    PORTD &= ~(1 << PD7);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print("C ");

  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Prag:");
  lcd.print(tempThreshold);
  lcd.print("C ");

  lcd.print((PIND & (1 << PD7)) ? "ON " : "OFF");
}

byte spiTransfer(byte dataOut) {
  PORTB &= ~(1 << PB2);
  SPDR = dataOut;
  while (!(SPSR & (1 << SPIF)));
  byte dataIn = SPDR;
  PORTB |= (1 << PB2);
  delay(50);
  return dataIn;
}

ISR(INT0_vect) { tempThreshold++; }
ISR(INT1_vect) { tempThreshold--; }
