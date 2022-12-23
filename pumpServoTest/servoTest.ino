#include <Servo.h>

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

Servo servo[12];

void setup() {
  // put your setup code here, to run once:

  pinMode(2, OUTPUT);

  pinMode(3, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode(19, OUTPUT);

  digitalWrite(2, LOW);

  servo[0].attach(3);
  servo[1].attach(6);
  servo[2].attach(9);
  servo[3].attach(10);
  servo[4].attach(12);
  servo[5].attach(13);
  servo[6].attach(14);
//  servo[7].attach(15);
//  servo[8].attach(16);
//  servo[9].attach(17);
//  servo[10].attach(18);
//  servo[11].attach(19);

  servo[0].write(180);
  servo[1].write(180);
  servo[2].write(180);
  servo[3].write(180);
  servo[4].write(180);
  servo[5].write(180);
  servo[6].write(180);
//  servo[7].write(180);
//  servo[8].write(180);
//  servo[9].write(180);
//  servo[10].write(180);
//  servo[11].write(180);
}

void loop() {
  // put your main code here, to run repeatedly:

}
