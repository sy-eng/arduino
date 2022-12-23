#include <Servo.h>
#include <EEPROM.h>

//PIN ASSIGN
//D0  : NC(RX)
//D1  : NC(TX)
//D2  : M0 ENABLE(ACTIVE LOW)
//D3  : M0 SIGNAL(0.5-2.5ms/20ms(50Hz), OC2B)
//D4  : M1 ENABLE(ACTIVE LOW)
//D5  : M1 SIGNAL(0.5-2.5ms/20ms(50Hz), OC0B)
//D6  : M2 SIGNAL(0.5-2.5ms/20ms(50Hz), OC0A)
//D7  : M2 ENABLE(ACTIVE LOW)
//D8  : M3 ENABLE(ACTIVE LOW)
//D9  : M3 SIGNAL(0.5-2.5ms/20ms(50Hz), OC1A)
//D10 : M4 SIGNAL(0.5-2.5ms/20ms(50Hz), OC1B)
//D11 : M4 ENABLE(ACTIVE LOW(OC2A, ICSP))
//D12 : NC(ICSP)
//D13 : NC(ICSP, LED ON BOARD)
//D14 : VALVE1 SIGNAL
//D15 : NC
//D16 : VALVE2 SIGNAL
//D17 : NC
//D18 : NC
//D19 : PUMP ON 
//A6  : NC
//A7  : NC
//
//FET FOR M ENABLE : IRFU5505PBF (PMOS)
//FET FOR PUMP ON : EKI04027(NMOS) 
//TR FOR VALVE SIGNAL : 2SC1815 (I_B = 20mA!)
//
//DO NOT TURN ON VALVE1 AND VALVE2 AT THE SAME TIME!!
//
// SYSTEM CLOCK : 16MHz
// Timer0 default settings : wiring.c : init() : 
//   TCCR0A : 0b00000011 (8bit fast PWM)
//   TCCR0B : 0b00000011 (1/64)
//   f0 = 16MHz / (64 * (255 + 1)) = 976.56Hz
//   This timer is used for delay() function!
// Timer1 default settings : wiring.c : init() : 
//   TCCR1A : 0b00000001 (8bit phase correct PWM)
//   TCCR1B : 0b00000011 (1/64)
//   f1 = 16MHz / (2 * 64 * 255) = 490.19Hz
// Timer2 default settings : wiring.c : init() : 
//   TCCR2A : 0b00000001 (8bit phase correct PWM)
//   TCCR2B : 0b00000100 (1/64)
//   f1 = 16MHz / (2 * 64 * 255) = 490.19Hz
// 
// f0 and f2 must be 16MHz / (2 * 1024 *256) = 30.5 (Phase correct PWM)...
//
// In servo library, TCNT1 counts as normal mode.
// There will an interrupt on the compare match with OCR1A.
// In the interrupt handler, clear the current pin, and set the next pin,
// and OCR1A is set TCNT + (tick for next channel).
// If all channel is scanned, TCNT = 0.
// Max pulse width for a pin is 2.5ms and REFRESH_INTERVAL is 20ms.
// SERVOS_PER_TIMER is 12. It looks too many...

#define ON 1
#define OFF 0
#define ENABLE 0
#define DISABLE 1

#define M0ENABLE 2
#define M0SIGNAL 3
#define M1ENABLE 4
#define M1SIGNAL 5
#define M2SIGNAL 6
#define M2ENABLE 7
#define M3ENABLE 8
#define M3SIGNAL 9
#define M4SIGNAL 10
#define M4ENABLE 11
#define LED 13
#define VALVE1SIGNAL 14
#define VALVE2SIGNAL 16
#define PUMP_ON 19

#define VALVE2 2
#define VALVE1 1
#define VALVE_OFF 0

#define BAUDRATE 9600

#define NEW_LINE 0x0A

#define SET_ANGLE 2
#define ONOFF 1

#define SERVO_NUM 5
#define MAX_WIDTH 2500
#define MIN_WIDTH 500
Servo servo[SERVO_NUM];

void valve(int valve){
  switch(valve){
    case VALVE1:
      digitalWrite(VALVE1SIGNAL, ON);
      digitalWrite(VALVE2SIGNAL, OFF);
      break;
    case VALVE2:
      digitalWrite(VALVE1SIGNAL, OFF);
      digitalWrite(VALVE2SIGNAL, ON);
      break;
    case VALVE_OFF:
      digitalWrite(VALVE1SIGNAL, OFF);
      digitalWrite(VALVE2SIGNAL, OFF);
      break;
    default:
      // Do nothing
      break;
  }
}

void pump(int pump){
  digitalWrite(PUMP_ON, pump);
}

void servoFunc(){
  int channel = -1;
  int value = -1;
  char key;

  while(1){
    if(Serial.available()){
      key = Serial.read();

      if(key >= '0' && key <= '9'){
        if(channel < 0){
          channel = (int)(key - '0');
        }else{
          if(value < 0){
            value = 0;
          }
          value = value * 10 + ((int)(key - '0'));
        }
      }else if(key == NEW_LINE){
        if(value >= 0 && channel < SERVO_NUM){
          servo[channel].write(value);
        }else{
          Serial.println("Something is wrong.");
        }
        break;
      }else{
        Serial.println("Value error.");
        break;
      }
    }
  }
}

void setup() {
  Serial.begin(BAUDRATE);

  pinMode(M0ENABLE, OUTPUT);
  pinMode(M0SIGNAL, OUTPUT);  
  pinMode(M1ENABLE, OUTPUT);
  pinMode(M1SIGNAL, OUTPUT);  
  pinMode(M2ENABLE, OUTPUT);
  pinMode(M2SIGNAL, OUTPUT);  
  pinMode(M3ENABLE, OUTPUT);
  pinMode(M3SIGNAL, OUTPUT);  
  pinMode(M4ENABLE, OUTPUT);
  pinMode(M4SIGNAL, OUTPUT);  
  pinMode(VALVE1SIGNAL, OUTPUT);
  pinMode(VALVE2SIGNAL, OUTPUT);  
  pinMode(PUMP_ON, OUTPUT);  

  digitalWrite(M0ENABLE, DISABLE);
  digitalWrite(M1ENABLE, DISABLE);
  digitalWrite(M2ENABLE, DISABLE);
  digitalWrite(M3ENABLE, DISABLE);
  digitalWrite(M4ENABLE, DISABLE);
  
  valve(VALVE_OFF);
  digitalWrite(PUMP_ON, OFF);

  // Turn on LED on board
  pinMode(LED, OUTPUT);
  digitalWrite(LED, ON);

  servo[0].attach(M0SIGNAL, MIN_WIDTH, MAX_WIDTH);
  servo[1].attach(M1SIGNAL, MIN_WIDTH, MAX_WIDTH);
  servo[2].attach(M2SIGNAL, MIN_WIDTH, MAX_WIDTH);
  servo[3].attach(M3SIGNAL, MIN_WIDTH, MAX_WIDTH);
  servo[4].attach(M4SIGNAL, MIN_WIDTH, MAX_WIDTH);

  servo[0].write(0);
  servo[1].write(0);
  servo[2].write(0);
  servo[3].write(0);
  servo[4].write(0);

  digitalWrite(M0ENABLE, ENABLE);
  digitalWrite(M1ENABLE, ENABLE);
  digitalWrite(M2ENABLE, ENABLE);
  digitalWrite(M3ENABLE, ENABLE);
  digitalWrite(M4ENABLE, ENABLE);

  Serial.println(EEPROM.length());
}

void loop() {
  char key;

  if(Serial.available()){
    key = Serial.read();

    if(key != NEW_LINE){
      Serial.write(key);
      Serial.write(':');
      Serial.write(NEW_LINE);
    }

    switch(key){
      case 'P':
        pump(ON);
        break;
      case 'p':
        pump(OFF);
        break;

      case 'l':
        valve(VALVE1);
        break;
      case 'r':
        valve(VALVE2);
        break;
      case 'f':
        valve(VALVE_OFF);
        break;

      case 'S':
        servoFunc();
        break;

      case NEW_LINE:
        break;

      default:
        Serial.println("Command error");
        break;
    }

  }
}
