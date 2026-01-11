  #define SS_PIN   10
  #define MOSI_PIN 11
  #define MISO_PIN 12
  #define SCK_PIN  13

  volatile byte command = 0;
  volatile byte dataToSend = 0;
  volatile uint8_t setpoint = 10;
  float fakeTemp = 22.0;
  float fakeHum = 55.0;
  int heaterOFF=1;
  // PID state
  float Kp = 10;
  float Ki = 10;
  float Kd = 8.0;

  float integ = 0.0;
  float prevErr = 0.0;

  volatile uint8_t heaterU = 0; // 0..255 output (for master to read)

  ISR(SPI_STC_vect) {
    command = SPDR;
    uint8_t rx = SPDR;
    if(rx <=60 && rx >0){
      setpoint = rx;
      SPDR = 0; ;
      heaterOFF=0;
      return;
    }
    if (command == 0x81)
      dataToSend = (byte)fakeTemp;
    else if (command == 0x82)
      dataToSend = (byte)fakeHum;
    else if (command == 0x83)
      heaterOFF = 1;
    SPDR = dataToSend;
  }

  void setup() {
    Serial.begin(9600);
    DDRB |= (1 << PB4);
    DDRB &= ~((1 << PB3) | (1 << PB5) | (1 << PB2));
    SPCR = (1 << SPE) | (1 << SPIE);
  }

  void loop() {
    const float dt = 0.15;

    // PID compute
    heaterU = pidCompute(fakeTemp, (float)setpoint, dt);

    // Plant simulation: temperature responds to heaterU
    const float ambient = 21.0;
    float power = heaterU / 255.0;
    
    float heating = power * 0.35;// heater strength
    if(heaterOFF==1) heating=0;                 
    float cooling = (fakeTemp - ambient) * 0.005;

    fakeTemp = fakeTemp + heating - cooling;
    Serial.println(fakeTemp);
    if (fakeTemp < 10) fakeTemp = 10;
    if (fakeTemp > 50) fakeTemp = 50;

   
  }

  static inline uint8_t pidCompute(float tempNow, float sp, float dt) {
  float err = sp - tempNow;


  // only heat if below setpoint
  if (err <= 0) {
    integ = 0;
    prevErr = 0;
    return 0;
  }

  integ += err * dt;
  if (integ > 60.0) integ = 60.0;
  if (integ < 0.0)  integ = 0.0;

  float deriv = (err - prevErr) / dt;
  prevErr = err;

  float u = Kp * err + Ki * integ + Kd * deriv;

  if (u < 0) u = 0;
  if (u > 255) u = 255;
  return (uint8_t)u;
}
