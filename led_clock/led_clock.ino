
byte minute = 8;
byte tenminute = 4;
byte hour = 3;
byte tenhour = 1;

long blinkInterval = 1000;
long prevMillis;
long currentMillis;





void setup() {
  DDRD = 0xff;
  PORTD = 0x00;
  
  DDRB = 0x02;
  PORTB = 0xfd;
}

void loop() {
  dispTime();
}

void dispTime() {
  
  currentMillis = millis();
  if(currentMillis - prevMillis >= blinkInterval) {
    prevMillis = currentMillis;
    PORTB = PINB ^ 0x02;
  }
  
  
  
  
  
  
  PORTD = 0b00010000 ^ minute;
  delay(1);
  PORTD = 0b00100000 ^ tenminute;
  delay(1);
  PORTD = 0b01000000 ^ hour;
  delay(1);
  PORTD = 0b10000000 ^ tenhour;
  delay(1);
}