


#include <avr/power.h>
#include <Bounce2.h>
#include <I2C.h>

#define down_buttonPin A0
#define up_buttonPin A1

#define rtcAddr 0x68

Bounce down_button = Bounce();
Bounce up_button = Bounce();

Bounce debouncer = Bounce(); 

boolean down_buttonChanged;
boolean up_buttonChanged;

boolean down_buttonState;
boolean up_buttonState;

unsigned long down_buttonPressTimeStamp;
unsigned long up_buttonPressTimeStamp;

unsigned long down_buttonPressTimeStart;
unsigned long up_buttonPressTimeStart;

byte minute = 0;
byte tenMinute = 0;
byte hour = 0;
byte tenHour = 0;

long blinkInterval = 1000;
unsigned long prevMillis;
unsigned long currentMillis;


void setup() {
  //disable adc
  power_adc_disable();

  DDRB = 0x02;
  PORTB = 0xfd;
  
  DDRC = 0x00;
  PORTB = 0xfc;
  
  DDRD = 0xff;
  PORTD = 0x00;
  
  //set up debouncer
  down_button.attach(down_buttonPin);
  down_button.interval(1);
  
  up_button.attach(up_buttonPin);
  up_button.interval(1);
  
  //initialize I2c
  I2c.begin();
  I2c.setSpeed(0);
  I2c.pullup(0);
  I2c.timeOut(0);
  
  //setup DS1307 RTC
  I2c.write(rtcAddr, 0x00);
  I2c.read(rtcAddr, 1);
  //enable the clock
  I2c.write(rtcAddr, 0x00, I2c.receive() & 0b01111111);
  //disable squarewave output
  I2c.write(rtcAddr, 0x07, 0x00);
  getTime();
}

void loop() {

  currentMillis = millis();
  
  //update debouncer
  down_buttonChanged = down_button.update();
  up_buttonChanged = up_button.update();
  
  if(down_buttonChanged) {
    if(down_button.read()) {
      down_buttonState = 0;
      
      setTime();
      
    }else {
      down_buttonState = 1;
      down_buttonPressTimeStamp = currentMillis;
      down_buttonPressTimeStart = currentMillis;
      decrMinute();
    }
  }
  
  if(up_buttonChanged) {
    if(up_button.read()) {
      up_buttonState = 0;
      
      setTime();
      
    }else {
      up_buttonState = 1;
      up_buttonPressTimeStamp = currentMillis;
      up_buttonPressTimeStart = currentMillis;;
      incrMinute();
    }
  }
  
  if  (down_buttonState == 1) {
    if(currentMillis - down_buttonPressTimeStart >= 3500) {
      if (currentMillis - down_buttonPressTimeStamp >= 7) {
        down_buttonPressTimeStamp = currentMillis;
        decrMinute();
      }
    }else if(currentMillis - down_buttonPressTimeStart >= 1500) {
      if (currentMillis - down_buttonPressTimeStamp >= 100) {
        down_buttonPressTimeStamp = currentMillis;
        decrMinute();
      }
    }else {
      if (currentMillis - down_buttonPressTimeStamp >= 400) {
        down_buttonPressTimeStamp = currentMillis;
        decrMinute();
      }
    }
  }
  
  if  ( up_buttonState == 1 ) {
    if(currentMillis - up_buttonPressTimeStart >= 3500) {
      if ( currentMillis - up_buttonPressTimeStamp >= 7) {
        up_buttonPressTimeStamp = currentMillis;
        incrMinute();       
      }
    }else if(currentMillis - up_buttonPressTimeStart >= 1500) {
      if (currentMillis - up_buttonPressTimeStamp >= 100) {
        up_buttonPressTimeStamp = currentMillis;
        incrMinute();       
      }
    }else {
      if (currentMillis - up_buttonPressTimeStamp >= 400) {
        up_buttonPressTimeStamp = currentMillis;
        incrMinute();       
      }
    }
  }
  
  if(currentMillis - prevMillis >= blinkInterval) {
    prevMillis = currentMillis;
    
    if(down_buttonState == 0 && up_buttonState == 0) {
      getTime();
    }
    
    //blink the doublepoint at 0.5Hz
    PORTB = PINB ^ 0x02;
  }
 
  dispTime();
}

void dispTime() {
  //display the time
  PORTD = 0b00010000 ^ minute;
  delay(1);
  PORTD = 0b00100000 ^ tenMinute;
  delay(1);
  PORTD = 0b01000000 ^ hour;
  delay(1);
  if(tenHour == 0x00) {
    PORTD = 0x00;
  }else {
    PORTD = 0b10000000 ^ tenHour;
  }
  delay(1);
}

void incrMinute(){
  if(minute < 9) {
    minute++;
  }else {
    minute = 0;
    if(tenMinute < 5){
      tenMinute++;
    }else {
      tenMinute = 0;
      incrHour();
    }
  }
}

void decrMinute() {
  if(minute > 0) {
    minute--;
  }else {
    minute = 9;
    if(tenMinute > 0){
      tenMinute--;
    }else {
      tenMinute = 5;
      decrHour();
    }
  }
}

void incrHour() {
  if(tenHour < 2) {
    if(hour < 9) {
      hour++;
    }else {
      hour = 0;
      tenHour++;
    }
  }else {
    if(hour < 3){
      hour++;
    }else{
      hour = 0;
      tenHour = 0;
    }
  }
}

void decrHour() {
  if(hour > 0){
    hour--;
  }else {
    if(tenHour > 0){
      tenHour--;
      hour = 9;
    }else {
      tenHour = 2;
      hour = 3;
    }
  }
}

void setTime() {
  int rtcMinute;
  int rtcHour;
  
  rtcMinute = (tenMinute << 4) ^ minute;
  rtcHour = (tenHour << 4) ^ hour;
  
  I2c.write(rtcAddr, 0x00, 0x00);
  I2c.write(rtcAddr, 0x01, rtcMinute);
  I2c.write(rtcAddr, 0x02, rtcHour);
}

void getTime() {
  int rtcMinute;
  int rtcHour;
  
  //read the current time
  I2c.write(rtcAddr, 0x01);
  I2c.read(rtcAddr, 2);
  
  rtcMinute = I2c.receive();
  rtcHour = I2c.receive();
  
  minute = rtcMinute & 0x0f;
  tenMinute = rtcMinute >> 4;
  hour = rtcHour & 0x0f;
  tenHour = rtcHour >> 4;
}
