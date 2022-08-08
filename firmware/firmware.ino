#include <AccelStepper.h>
AccelStepper stepper(AccelStepper::DRIVER, A2, A3); // Defaults to AccelStepper::FULL4WIRE (4 pins) on 2, 3, 4, 5

volatile int trig = 0;
bool moving = false;
bool SW_LIMIT_EN;

void HW_OVERTRAVEL()
{
  stepper.setCurrentPosition(0);
  trig = 1;
}

void zero_axis()
{
  while( !trig )
  {
    stepper.moveTo(stepper.currentPosition() - 1000);
    stepper.run();
  }
  
  Serial.println("r");
}

void setup()
{   
  
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(2, INPUT);

  stepper.setMaxSpeed(2500);
  stepper.setAcceleration(2500);
  attachInterrupt(digitalPinToInterrupt(2), HW_OVERTRAVEL, FALLING);
  Serial.begin(9600);
  
  if(digitalRead(2))
    zero_axis();
  
  SW_LIMIT_EN = true;
}

void loop()
{
  
    if( ( (stepper.currentPosition() < 1000) || (stepper.currentPosition() >  30000)) && SW_LIMIT_EN)
    {
      trig = 2;
    }
  
    switch(trig)
    {
      case 1:
      {
        Serial.println("h");
        trig = 2;
        break;
      }
      case 2:
      {
        trig = 0;
        if(stepper.currentPosition() < 1000)
        {
          stepper.moveTo(1000); 
        }
        else if(stepper.currentPosition() > 33000)
        {
          stepper.moveTo(33000);
        }
        
        //Serial.println("done");
        break;
      }
    }
    unsigned long long target;

    while ( Serial.available() )
    {
      //Serial.println("here");
      target = Serial.parseInt() / 25.0f * 1000;

      if( target == 10 )
        continue;
      
      if ( ( target < 1000 ) || ( target > 33000 ))
      {
        //Serial.println("s");
        Serial.println((int)target);
      }
      else
      {
        stepper.moveTo( target );
        moving = true;
      }
      Serial.println((int)target);
   }

   if((!stepper.distanceToGo()) && (moving) )
   {
      Serial.println("d");
      moving = false;
   }
   stepper.run();
}
