// THIS CODE CONTROLS A POLOLU TB67S128FTG BOARD WITH ARDUINO NANO.
// THE STEPPING MOTOR CONNECTED TO THE BOARD IS 55SPM25D7YC.
// 
// PIN ASSIGN (ARDUINO NANO)
// D0  : NC (RX)
// D1  : NC (TX)
// D2  : GAIN_SEL
// D3  : IF_SEL
// D4  : LO1
// D5  : LO0
// D6  : MO
// D7  : RESET
// D8  : INV(STANDBY)
// D9  : MODE2
// D10 : ENABLE
// D11 : CW/CCW (MOSI(SPI))
// D12 : MODE1  (MISO(SPI), IN SERIAL MODE, THIS PIIN IS NOT NC)
// D13 : CLK    (SCK(SPI))
// D14 : MODE0  (A0)
// D15 : TORQE2 (A1)
// D16 : TORQE1 (A2)
// D17 : TORQE0 (A3)
// D18 : MDT1   (A4)
// D19 : MDT0   (A5)
// A6  : NC
// A7  : NC
// 
// PIN ASSIGN (55SPM25D7YC, 2-PHASE UNIPOLAR STEPPING MOTOR)
// YELLOW : 1(A+)
// BLACK  : 5(A_COMMON)
// BLUE   : 2(A-)
// RED    : 4(B+)
// WHITE  : 6(B_COMMON)
// BROWN  : 3(B-)

// EDGE_SEL : NC : SINGLE EDGE (UP-EDGE)
// RS_SEL : NC : INTERNAL CURRENT FEED BACK (WITHOUT RS RESISTER)
// AGC : NC  : AGC: OFF
// 
// THESE PINS ARE NOT ACTIVE IF AGC IS OFF.
// CLIM1 : NC
// CLIM1 : 100k? PULLED-UP
// IF AGC IS ON, AGC bottom current limit: Iout × 55%
//  
// FLIM : 100k? PULLED-UP :
// Frequency limit: ON, AGC is invalid when fCLK is below 450 Hz
// THIS PIN IS FOR AGC. 
// 
// BOOST : 100k? PULLED-UP : 
// Takes 7 steps maximum (Design value)
// THIS PIN IS FOR AGC.
//
// LTH : 100k RESISTER IS CONNECTED.

// GAIN_SEL : OUTPUT
// THIS PIN IS NOT USED IN SERIAL MODE.
// L : CURRENT GAIN 1/5
// H : CURRENT GAIN 1/10 (HALF OF L)
#define GAIN_SEL 2

// IF_SEL : OUTPUT
// L : CLK MODE
// H : SERIAL MODE
#define IF_SEL 3

// LOx : INPUT(OPEN COLLECTOR, 100k PULLED-UP)
// 0 : NORMAL
// 1 : MOTOR IS NOT CONNECTED
// 2 : OVERCURRENT
// 3 : THERMAL SHUTDOWN
#define LO1 4
#define LO0 5

// MO : INPUT(OPEN COLLECTOR, 100k PULLED-UP)
// Z PHASE
#define MO 6

// RESET : OUTPUT
// THIS PIN IS USED FOR BANK_EN IN SERIAL MODE
// IN CLK MODE, THIS PIN INITIALIZE THE INTERNAL ELECTRICAL ANGLE
// AT 45 DEGREE (71%, 71%). 
// L : NORMAL
// H : RESET
#define RESET 7

// INV(STANDBY) : OUTPUT
// L : STANDBY MODE
// H : NORMAL MODE
#define INV_STANDBY 8

// MODEx : OUTPUT
// THESE PINS ARE NC IN SERIAL MODE.
// MODE1 IS USED FOR MISO IN SPI AND IN SERIAL MODE.
// THESE PINS DECIDE RESOLUTION. 
// (0 : 1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64, 7 : 1/128)
#define MODE2 9
#define MODE1 12
#define MODE0 14

// ENABLE : OUTPUT
// THIS PIN IS USED FOR LATCH IN SERIAL MODE
// IN CLK MODE
// L : OUTPUT DISABLE
// H : OUTPUT ENABLE
#define ENABLE 10

// CWCCW : OUTPUT
// THIS PIN IS USED FOR DATA IN SERIAL MODE
// IN CLK MODE, THIS PIN DECIDES THE ROTATION DIRECTION.
// OUTPUT PINS ARE INITIALIZED AT 90 DEGREE. (H OR L)
#define CWCCW 11

// CLK : OUTPUT
#define CLK 13

// TORQEx : OUTPUT
// THESE PINS ARE NC IN SERIAL MODE.
// 0 : SET TORQUE 100%, 85%, 70%, 60%, 50%, 40%, 25%, 7 : 10%
#define TORQE2 15
#define TORQE1 16
#define TORQE0 17

// MDTx : OUTPUT
// THESE PINS ARE NC IN SERIAL MODE.
// 0 : FAST DECAY 37.5%, 1 : FAST DECAY 50%,
// 2 : FAST DECAY ONLY, 3 : ADMD
#define MDT1 18
#define MDT0 19

#define TRUE 1
#define FALSE 0

#define CW FALSE
#define CCW TRUE

#define UP 1
#define DOWN -1

#define BAUDRATE 9600

#define NEW_LINE 0x0A

char stopFlag = FALSE;
char signalFlag = FALSE;

char getSignal(){
  char retVal;

  //retVal = (char)((PORTD >> 4) & 0b00000111);
  retVal = (char)(digitalRead(MO)*4 + digitalRead(LO1)*2 + digitalRead(LO1));

  // 0 : LO1, 1 : LO0, 2 : MO  
  return retVal;
}

char getMode(){
  char retVal = 0;
  retVal = (char)(digitalRead(MODE2) * 4 + digitalRead(MODE1) * 2 + digitalRead(MODE0));

  return retVal;
}

void setMode(char mode){
  if(mode & 0b00000100){
    //Serial.write('H');
    digitalWrite(MODE2, HIGH);
  }else{
    //Serial.write('L');
    digitalWrite(MODE2, LOW);
  }
  if(mode & 0b00000010){
    //Serial.write('H');
    digitalWrite(MODE1, HIGH);
  }else{
    //Serial.write('L');
    digitalWrite(MODE1, LOW);
  }
  if(mode & 0b00000001){
    //Serial.write('H');
    digitalWrite(MODE0, HIGH);
  }else{
    //Serial.write('L');
    digitalWrite(MODE0, LOW);
  }

//  digitalWrite(RESET, HIGH);
//  delay(10);
//  digitalWrite(RESET, LOW);

}

void updownMode(int updown){
  char mode;

  mode = getMode();

  mode -= updown;

  if(mode > 7){
    mode = 7;
  }else if(mode < 0){
    mode = 0;
  }

  //Serial.write(mode + '0');

  setMode(mode);
}

void setup() {
  pinMode(GAIN_SEL, OUTPUT);
  pinMode(IF_SEL, OUTPUT);
  pinMode(LO1, INPUT);
  pinMode(LO0, INPUT);
  pinMode(MO, INPUT);
  pinMode(RESET, OUTPUT);
  pinMode(INV_STANDBY, OUTPUT);
  pinMode(MODE2, OUTPUT);
  pinMode(MODE1, OUTPUT);
  pinMode(MODE0, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(CWCCW, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(TORQE2, OUTPUT);
  pinMode(TORQE1, OUTPUT);
  pinMode(TORQE0, OUTPUT);
  pinMode(MDT1, OUTPUT);
  pinMode(MDT0, OUTPUT);

  digitalWrite(CLK, LOW);
  digitalWrite(CWCCW, CW);

  // SET GAIN 1/10
  digitalWrite(GAIN_SEL, HIGH);
  // SET CLK MODE
  digitalWrite(IF_SEL, LOW);

  // SET MODE : 1
  digitalWrite(MODE0, LOW);
  digitalWrite(MODE1, LOW);
  digitalWrite(MODE2, LOW);

  // SET TORQUE 100%
  // SET TORQUE AND GAIN_SEL COLLECTLY, OR VOLTAGE WILL SATULATE!
//  digitalWrite(TORQE0, LOW);
//  digitalWrite(TORQE1, LOW);
//  digitalWrite(TORQE2, LOW);
  digitalWrite(TORQE0, HIGH);
  digitalWrite(TORQE1, HIGH);
  digitalWrite(TORQE2, HIGH);
  // FAST DECAY 37.5%
  digitalWrite(MDT0, LOW);
  digitalWrite(MDT1, LOW);

  digitalWrite(RESET, LOW);
  digitalWrite(INV_STANDBY, HIGH);
  digitalWrite(ENABLE, HIGH);

  TCCR1A = 0;
  TCCR1B = 0;
  // NO PIN IS FOR PWM OUTPUT.
  // MODE : CTC(TOP : OCR1A)
  // PRESCALER : 256
  TCCR1B |= (1 << WGM12) | (1 << CS12);
  // CLOCK : 16MHz, INT_FREQ = 16MHz/(PRESCALER*(1 + OCR1A))
  // OCR1A = 16MHz / (PRESCALER * INT_FREQ) - 1
  // THIS SETTING IS 2Hz
  OCR1A = 31249;
  // INTERRUPT IS ENABLED
  TIMSK1 |= (1 << OCIE1A);

  Serial.begin(BAUDRATE);
}

ISR(TIMER1_COMPA_vect){
  char sig;

  if(stopFlag == FALSE){
    if(digitalRead(CLK)){
      digitalWrite(CLK, LOW);
    }else{
      digitalWrite(CLK, HIGH);
    }
  }

}

void getInterval(){
  unsigned int data = OCR1A;
  char val[5];

  Serial.write((data / 10000) + '0');
  data = data % 10000;
  Serial.write((data / 1000) + '0');
  data = data % 1000;
  Serial.write((data / 100) + '0');
  data = data % 100;
  Serial.write((data / 10) + '0');
  data = data % 10;
  Serial.write(data + '0');
  Serial.println("");
}

void setInterval(){
  char key;
  unsigned int data = 0;

  while(1){
    if(Serial.available()){
      key = Serial.read();
    
      if(key >= '0' && key <= '9'){
        data = data * 10 + (key - '0');
      }else if(key == NEW_LINE){
        OCR1A = data;
        getInterval();
        break;
      }else{
        Serial.println("Format error.");
        break;
      }
    }
  }  
}

void help(){
  Serial.println("THIS CODE CONTROLS A POLOLU TB67S128FTG BOARD WITH ARDUINO NANO.");
  Serial.println("THE STEPPING MOTOR CONNECTED TO THE BOARD IS 55SPM25D7YC.");
  Serial.println(" ");
  Serial.println("USAGE : ");
  Serial.println("l : Set rotation direction in CCW.");
  Serial.println("r : Set rotation direction in CW.");
  Serial.println("s : Toggle motor start / stop.");
  Serial.println("u : Make motor prescaler resolution higher. (Max : 1/128)");
  Serial.println("d : Make motor prescaler resolution lower. (Max : 1)");
  Serial.println("h : Show help.");
}

void loop() {
  if(Serial.available()){
    char key;

    key = Serial.read();

    switch(key){
      case 'l':
        digitalWrite(CWCCW, CCW);
        break;
      case 'r':
        digitalWrite(CWCCW, CW);
        break;
      case 's':
        if(stopFlag == TRUE){
          stopFlag = FALSE;
        }else{
          stopFlag = TRUE;
        }
      case 'u':
        updownMode(UP);
        break;
      case 'd':
        updownMode(DOWN);
        break;
      case 'm':
        Serial.write(getMode() + '0');
        Serial.println("");
        break;
      case 'g':
        Serial.write(getSignal() + '0');
        Serial.println("");
        break;
      case 'p':
        setInterval();
        break;
      case 'P':
        getInterval();
        break;
      case 'h':
        help();
        break;
      default:
        break;
    }
  }
}
