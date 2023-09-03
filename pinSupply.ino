/*
This program is for switching the power supply for the pin.
2      : OUT : 
13     : OUT : Switch the pin. If this pin is ON, the power will be supplied to the pin.
14(A0) : OUT : Supply the pin 24V, but its current is about 10mA.
A1     : IN  : Valve signal. This pin is analog input. Sampling rate is 10kHz as default.
A6     : IN  : Check the proximity switch status. This pin is used as analog input
A7     : IN  : Check the voltage of pin OUTPUT. This pin is used when 14 pin is used. 
               If this pin is not used, the mechanical switches should be turned off.

Anyway, 13 must be used. At first, 15 pin is used for valve monitoring.
*/

// define on-delay in ms
#define ON_DELAY 1000
// define off-delay in ms
//#define OFF_DELAY 100

#include <TimerOne.h>

#define ON 1
#define OFF 0
#define VALVE_THRESHOLD 200

long onDelayCount = ON_DELAY * 1;
//long offDelayCount = OFF_DELAY * 1; //This can be 0ms
uint8_t pinOutputStatus = LOW;
uint8_t valveOutputStatus = LOW;
long delayCount = -1;
int prevValveStatus = -1;

int readValveStatus(){
  int retVal;

  // It takes 0.15ms for analogRead().
  if(analogRead(1) < VALVE_THRESHOLD){
    retVal = ON;
  }else{
    retVal = OFF;
  }
}

void resetOutput(){
  pinOutputStatus = LOW;
  digitalWrite(13, pinOutputStatus);
  valveOutputStatus = LOW;
  digitalWrite(2, valveOutputStatus);
}

// This function is called every 1ms
void timerFunc(){
  int valveStatus = readValveStatus();
  if(prevValveStatus == -1){
    prevValveStatus = valveStatus;
  }else{
    if(delayCount < 0){
      if(prevValveStatus != valveStatus){

        if(pinOutputStatus == HIGH){
          if(valveStatus != (int)pinOutputStatus && valveStatus != (int)valveOutputStatus){
            delayCount = 0;
            valveOutputStatus = HIGH;
            digitalWrite(2, valveOutputStatus);
          }else{
            //something is wrong
            resetOutput();
          }
        }else{
          resetOutput();
        }
      }
    }else{
      //Here, valveStatus should be ON, pinOutputStatus should be LOW, 
      //valveOutputStatus should be HIGH
      if(valveStatus == ON){
        delayCount++;

        if(delayCount == onDelayCount){
          pinOutputStatus = HIGH;
          digitalWrite(13, pinOutputStatus);
        }
      }else{
        //something is wrong
        delayCount = -1;
        resetOutput();
      }
    }

    prevValveStatus = valveStatus;
  }

  return;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);
  digitalWrite(2, valveOutputStatus);
  pinMode(13, OUTPUT);
  digitalWrite(13, pinOutputStatus);
 
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerFunc);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Do nothing
}
