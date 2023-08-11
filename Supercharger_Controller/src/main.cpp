#include <Arduino.h>

/*
  1988 Toyota MR2 4AGZE Supercharger Controller
     Controls supercharger clutch on/off for 3 different drive modes,
     selected by dash mounted rotary switch in voltage divider configuration.
       Modes:
        1. Supercharger always off
        2. On during acclerations until turbo spools (pre-throttlebody pressure[PTB] > 9PSIG)
        3. On anytime PTB is less then 9PSIG & RPMs is below low TH angle limit
 */

#define DashSwitch A0 // select the input pin for the dash mounted mode switch
#define ScRelay 2     // select the output pin for supercharger output relay
#define ScEcuInt 3    // select the output pin for supercharge on/off output indicator to ECU
#define PTB 6         // select the input pin for pre-throttlebody air pressure from MS3
#define TPSDot 7      // select the input pin for TPSdot (acceleration) from MS3
#define NELimit 8     // select the input pin for NE limit at low TH from MS3

int DashSwitchVal = 0; // variable to store DashSwitch read value
int ScRelayVal = 0;    // variable to store ScRelay output value
int PTBVal = 0;        // variable to store PTB read value
int TPSDotVal = 0;     // variable to store TPSDot read value
int NELimitVal = 0;    // variable to store NELimit read value

void setup()
{
  // declare the pins as INPUT or OUTPUT:
  pinMode(ScRelay, OUTPUT);
  pinMode(ScEcuInt, OUTPUT);
  pinMode(PTB, INPUT);
  pinMode(TPSDot, INPUT);
  pinMode(NELimit, INPUT);
}

void loop()
{

  // read the values from the input pins:
  DashSwitchVal = analogRead(DashSwitch);
  PTBVal = digitalRead(PTB);
  TPSDotVal = digitalRead(TPSDot);
  NELimitVal = digitalRead(NELimit);

  // control logic if dash switch is in position 1:
  if (DashSwitchVal <= 50)
  {
    digitalWrite(ScRelay, LOW);
    ScRelayVal = 0;
  }
  // control logic if dash switch is in position 2:
  else if (DashSwitchVal > 50 && DashSwitchVal <= 700)
  {
    if (PTBVal == LOW && TPSDotVal == HIGH && NELimitVal == LOW)
    {
      digitalWrite(ScRelay, HIGH);
      ScRelayVal = 1;
    }
    else
    {
      digitalWrite(ScRelay, LOW);
      ScRelayVal = 0;
    }
  }
  // control logic if dash switch is in position 3:
  else
  {
    if (PTBVal == LOW && NELimitVal == LOW)
    {
      digitalWrite(ScRelay, HIGH);
      ScRelayVal = 1;
    }
    else
    {
      digitalWrite(ScRelay, LOW);
      ScRelayVal = 0;
    }
  }

  // sets ScEcuInt the same as ScRelay:
  if (ScRelayVal == 1)
  {
    digitalWrite(ScEcuInt, HIGH);
  }
  else
  {
    digitalWrite(ScEcuInt, LOW);
  }
}
