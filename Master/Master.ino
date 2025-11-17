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
  
  DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);
  DDRB &= ~(1 << PB4);

  PORTB |= (1 << PB2);

  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

  
  DDRD |= (1 << PD7); //pin 7

  DDRD &= ~((1 << PD2) | (1 << PD3)); 
  PORTD |= (1 << PD2) | (1 << PD3);   
  EICRA |= (1 << ISC01) | (1 << ISC00); // INT0 
  EICRA |= (1 << ISC11) | (1 << ISC10); // INT1 
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

  if (temperature < tempThreshold)
    PORTD |= (1 << PD7);
  else
    PORTD &= ~(1 << PD7);

  delay(1000);
}

byte spiTransfer(byte dataOut) {
  PORTB &= ~(1 << PB2); // SS low
  SPDR = dataOut;       // send data
  while (!(SPSR & (1 << SPIF))); 
  byte dataIn = SPDR;
  PORTB |= (1 << PB2); // SS high
  delay(100);
  return dataIn;
}

// ISR pentru creștere prag-temperatura
ISR(INT0_vect) {
  tempThreshold++;
}

// ISR pentru scădere prag-temperatura
ISR(INT1_vect) {
  tempThreshold--;
}
