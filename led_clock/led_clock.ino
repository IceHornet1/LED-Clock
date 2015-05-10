
#include <avr/power.h>
#include <Bounce2.h>

#define h_buttonPin A0
#define min_buttonPin A1

Bounce h_button = Bounce();
Bounce min_button = Bounce();

Bounce debouncer = Bounce(); 

boolean h_buttonChanged;
boolean min_buttonChanged;

boolean h_buttonState;
boolean min_buttonState;

unsigned long h_buttonPressTimeStamp;
unsigned long min_buttonPressTimeStamp;

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
  
  pinMode(h_buttonPin,INPUT);
  pinMode(min_buttonPin,INPUT);
  
  DDRD = 0xff;
  PORTD = 0x00;
  
  //set up debouncer
  h_button.attach(h_buttonPin);
  h_button.interval(1);
  
  min_button.attach(min_buttonPin);
  min_button.interval(1);
}

void loop() {
  dispTime();
}

void dispTime() {
  
  currentMillis = millis();
  
  //update debouncer
  h_buttonChanged = h_button.update();
  min_buttonChanged = min_button.update();
  
  if(h_buttonChanged) {
    if(h_button.read()) {
      h_buttonState = 0;
      //update time here
      
    }else {
      h_buttonState = 1;
      h_buttonPressTimeStamp = currentMillis;
      incrHour();
    }
  }
  
  if(min_buttonChanged) {
    if(min_button.read()) {
      min_buttonState = 0;
      
      //update time here
      
    }else {
      min_buttonState = 1;
      min_buttonPressTimeStamp = currentMillis;
      incrMinute();
    }
  }
  
  if  ( h_buttonState == 1 ) {
    if ( currentMillis - h_buttonPressTimeStamp >= 500 ) {
      h_buttonPressTimeStamp = currentMillis;
      incrHour();
    }
  }
  
  if  ( min_buttonState == 1 ) {
    if ( currentMillis - min_buttonPressTimeStamp >= 100 ) {
      min_buttonPressTimeStamp = currentMillis;
      incrMinute();       
    }
  }
  
  
  
  if(currentMillis - prevMillis >= blinkInterval) {
    prevMillis = currentMillis;
    
    //update the time here
    
    //blink the doublepoint at 0.5Hz
    PORTB = PINB ^ 0x02;
  }
 
  //display the time
  PORTD = 0b00010000 ^ minute;
  delay(1);
  PORTD = 0b00100000 ^ tenMinute;
  delay(1);
  PORTD = 0b01000000 ^ hour;
  delay(1);
  PORTD = 0b10000000 ^ tenHour;
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
          incrHour;
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