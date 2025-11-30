  #define SS_PIN   10
  #define MOSI_PIN 11
  #define MISO_PIN 12
  #define SCK_PIN  13

  volatile byte command = 0;
  volatile byte dataToSend = 0;
  float fakeTemp = 22.0;
  float fakeHum = 55.0;

  ISR(SPI_STC_vect) {
    command = SPDR;
    fakeTemp += random(-1, 2) * 0.3;
    fakeHum += random(-1, 2);

    if (command == 0x01)
      dataToSend = (byte)fakeTemp;
    else if (command == 0x02)
      dataToSend = (byte)fakeHum;

    SPDR = dataToSend;
  }

  void setup() {
    DDRB |= (1 << PB4);
    DDRB &= ~((1 << PB3) | (1 << PB5) | (1 << PB2));
    SPCR = (1 << SPE) | (1 << SPIE);
  }

  void loop() {

  }
