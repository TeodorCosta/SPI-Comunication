  #define SS_PIN   10
  #define MOSI_PIN 11
  #define MISO_PIN 12
  #define SCK_PIN  13

  volatile byte command = 0;
  volatile byte dataToSend = 0;
  volatile uint8_t setpoint = 10;

  float fakeTemp = 22.0;
  float lastTemp =0;
  float fakeHum = 55.0;

  uint16_t humMsAccum = 0;
  const uint16_t HUM_UPDATE_MS = 200;
  float humRateUp   = 6.0;  // %RH per second when temp decreases
  float humRateDown = 15.0; // %RH per second when temp increases
  float humDeadband = 0.01; // degC

  int heaterOFF=1;

  // PID state
  float Kp = 10;
  float Ki = 10;
  float Kd = 8.0;

  float integ = 0.0;
  float prevErr = 0.0;

  volatile uint8_t heaterU = 0; // 0..255 output

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

    timer1_init();
    TCNT1 = 0;
  }

  void loop() {
    const float dt = 0.15;

    heaterU = pidCompute(fakeTemp, (float)setpoint, dt);

    const float ambient = 21.0;
    float power = heaterU / 255.0;
    
    float heating = power * 0.35;// heater strength
    if(heaterOFF==1) heating=0;                 
    float cooling = (fakeTemp - ambient) * 0.005;

    fakeTemp = fakeTemp + heating - cooling;
    Serial.println(fakeTemp);
    /// time accumulation from Timer1
    humMsAccum += timer1_elapsed_ms_and_reset();

if (humMsAccum >= HUM_UPDATE_MS) {
  float dt = humMsAccum / 1000.0;   // seconds since last humidity update
  humMsAccum = 0;

  float dT = fakeTemp - lastTemp;

  if (dT > humDeadband) {
    // temp increased => humidity decreases
    fakeHum -= humRateDown * dt;
  } 
  else if (dT < -humDeadband) {
    // temp decreased => humidity increases
    fakeHum += humRateUp * dt;
  }

  if (fakeHum < 20.0) fakeHum = 20.0;
  if (fakeHum > 90.0) fakeHum = 90.0;
}

  lastTemp = fakeTemp;
  }

  static inline uint8_t pidCompute(float tempNow, float sp, float dt) {
  float err = sp - tempNow;

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

static inline void timer1_init() {
  TCCR1A = 0;                               // normal mode
  TCCR1B = (1 << CS11) | (1 << CS10);       // prescaler 64
}

// returns elapsed ms since last call using Timer1 ticks
static inline uint16_t timer1_elapsed_ms_and_reset() {
  // 16MHz/64 = 250kHz => 1 tick = 4us
  // 1ms = 250 ticks
  uint16_t ticks = TCNT1;
  TCNT1 = 0;
  return (uint16_t)(ticks / 250);
}

