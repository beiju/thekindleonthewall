/* X10 RF Receiver Calibration                                             BroHogan 11/21/2010
 * SETUP:
 * 
 */


#include <X10rf.h>
//#include <Tone.h>
#define SPKR_PIN 7

X10rf x10rf = X10rf(0, 2, processRfCommand);
//Tone tone1;

void setup(){
//  tone1.begin(SPKR_PIN);
  Serial.begin(57600);
  x10rf.begin();
  Serial.println("JJG X10 Test");
}

void loop(){
}

// ISR called when RF received . . .
void processRfCommand(char house, byte unit, byte command, bool isRepeat){

//  tone1.play(2500,250);
  Serial.print("RF: ");
  Serial.print(house);
  Serial.print("-");
  Serial.print(unit,DEC);
  if (command == B0010) Serial.println(" ON");
  if (command == B0011) Serial.println(" OFF");
}




