#define SS_PIN   10
#define MOSI_PIN 11
#define MISO_PIN 12
#define SCK_PIN  13
#define HEATER_LED 7

#define BTN_UP 2
#define BTN_DOWN 3

volatile int tempThreshold = 25;
volatile float temperature = 0, humidity = 0;

void setup() {
  // ---------------- SPI Master setup ----------------
  // Set MOSI, SCK, SS as output
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  // MISO input
  DDRB &= ~(1 << PB4);

  // Deselect slave (SS high)
  PORTB |= (1 << PB2);

  // Enable SPI, Master mode, clock = fosc/16
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

  // ---------------- LED setup ----------------
  DDRD |= (1 << PD7); // LED output (pin 7)

  // ---------------- Buttons setup ----------------
  DDRD &= ~((1 << PD2) | (1 << PD3)); // Inputs
  PORTD |= (1 << PD2) | (1 << PD3);   // Pull-up

  // ---------------- Interrupts ----------------
  EICRA |= (1 << ISC01) | (1 << ISC00); // INT0 rising edge
  EICRA |= (1 << ISC11) | (1 << ISC10); // INT1 rising edge
  EIMSK |= (1 << INT0) | (1 << INT1);

  Serial.begin(9600);
}

void loop() {
  
  byte humData= spiTransfer(0x02);
  byte tempData = spiTransfer(0x01);
  temperature = tempData;
  humidity = humData;

  Serial.print("Temp=");
  Serial.print(temperature);      
  Serial.print("C  Hum=");
  Serial.print(humidity);
  Serial.print("%  Prag=");
  Serial.print(tempThreshold);
  Serial.print("C  Heater=");
  Serial.println((PIND & (1 << PD7)) ? "ON" : "OFF");

  // Control LED încălzire (PD7)
  if (temperature < tempThreshold)
    PORTD |= (1 << PD7);
  else
    PORTD &= ~(1 << PD7);

  delay(1000);
}

byte spiTransfer(byte dataOut) {
  PORTB &= ~(1 << PB2); // SS low
  SPDR = dataOut;       // send data
  while (!(SPSR & (1 << SPIF))); // wait
  byte dataIn = SPDR;
  PORTB |= (1 << PB2); // SS high
  delay(100);
  return dataIn;
}

// ISR pentru creștere prag
ISR(INT0_vect) {
  tempThreshold++;
}

// ISR pentru scădere prag
ISR(INT1_vect) {
  tempThreshold--;
}
