// C++ code
//
#include <SoftwareSerial.h>

// intercomm tx pin master 3
// intercom rx pin master 4
// viceversa on slave
// automatic status indicator 5

// intercomm commands
// 0: connection check, ack'd with 0
// 1: mode switch, ack'd with 1
// 2: 

int modePin = 2;

#define MOT1_EN 6
#define MOT1_DIR 8
#define MOT2_EN 7
#define MOT2_DIR 9

#define FWB A1
#define RVB A3
#define LFB A2
#define RFB A0



void setup()
{
  pinMode(MOT1_EN, OUTPUT);
  pinMode(MOT1_DIR, OUTPUT);
  pinMode(MOT2_EN, OUTPUT);
  pinMode(MOT2_DIR, OUTPUT);
  
  pinMode(FWB, INPUT_PULLUP);
  pinMode(RVB, INPUT_PULLUP);
  pinMode(LFB, INPUT_PULLUP);
  pinMode(RFB, INPUT_PULLUP);
  Serial.begin(9600);
}


void fw()
{
  digitalWrite(MOT1_EN, HIGH);
  digitalWrite(MOT2_EN, HIGH);
  digitalWrite(MOT1_DIR, HIGH);
  digitalWrite(MOT2_DIR, HIGH);
}


void bk()
{
  digitalWrite(MOT1_EN, HIGH);
  digitalWrite(MOT2_EN, HIGH);
  digitalWrite(MOT1_DIR, LOW);
  digitalWrite(MOT2_DIR, LOW);
}


void lt()
{
  digitalWrite(MOT1_EN, HIGH);
  digitalWrite(MOT2_EN, HIGH);
  digitalWrite(MOT1_DIR, HIGH);
  digitalWrite(MOT2_DIR, LOW);
}


void rt()
{
  digitalWrite(MOT1_EN, HIGH);
  digitalWrite(MOT2_EN, HIGH);
  digitalWrite(MOT1_DIR, LOW);
  digitalWrite(MOT2_DIR, HIGH);
}


void stp()
{
  digitalWrite(MOT1_EN, LOW);
  digitalWrite(MOT2_EN, LOW);
  digitalWrite(MOT1_DIR, LOW);
  digitalWrite(MOT2_DIR, LOW);
}

void loop()
{
 	uint8_t t = 0;
  	t = ( ( ( !digitalRead(FWB) ) << 1 ) | ( !digitalRead(RVB)) ) << 1;
  	t = (( t | !digitalRead(LFB) ) << 1 ) | !digitalRead(RFB);
  	
  	switch(t)
    {
      case 0b1000:
      	fw();
      	break;
      case 0b0100:
      	bk();
      	break;
      case 0b0010:
      	lt();
      	break;
      case 0b0001:
      	rt();
      	break;
      case 0b0000:
      	stp();
      	break;
    }
}