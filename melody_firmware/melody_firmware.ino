/**
 * ATtiny85 Melody Module
 * 
 * The board operates from a coincell battery (CR2032 3V 225mAh) and
 * plays a melody (piezo) when the trigger pin is HIGH. Can be triggered via
 * a reed switch or with the output headers from an external source.
 * 
 * Ando David Roots <david@sqroot.eu>
 * 2014-02-25
 * http://sqroot.eu/2014/02/attiny85-melody-module
 *
**/

// Power-saving functions
#include <JeeLib.h> 

// Setup watchdog
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// Tone frequencies
#include "pitches.h"

// ATtiny85 pin 0 as PWM for the buzzer
#define melodyPin 0

// Analog in pin for the trigger
#define triggerPin 3

// Mario main theme melody by Diptop Pratyaksa 31/3/13
int melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7, 
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0, 

  NOTE_C7, 0, 0, NOTE_G6, 
  0, 0, NOTE_E6, 0, 
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0, 

  NOTE_G6, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6, 
  0, 0, NOTE_E6, 0, 
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0, 

  NOTE_G6, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0, 0
};

// Mario main them tempo
int tempo[] = {
  12, 12, 12, 12, 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12, 

  12, 12, 12, 12,
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};


void setup(void)
{
   pinMode(melodyPin, OUTPUT); // Buzzer
   pinMode(triggerPin, INPUT); // Trigger
}

void loop()
{

  // Play melody when trigger is active
  if (isTriggerActive(true)) {
    sing();
  }
  
  // Otherwise, standby mode
  Sleepy::loseSomeTime(1000);
}

// Read the trigger pin and return true when it's high. Optionally, (software) debounce the reading
boolean isTriggerActive(boolean debounce){
  
  if (digitalRead(triggerPin) == HIGH) {
    if (!debounce) {
      return true;
    }
    
    delay(50);
    if (digitalRead(triggerPin) == HIGH) {
       return true; 
    }
  }
  
  return false;  
}


// Play melody
void sing(){      
     int size = sizeof(melody) / sizeof(int);
     for (int thisNote = 0; thisNote < size; thisNote++) {

       // to calculate the note duration, take one second
       // divided by the note type.
       //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
       int noteDuration = 1000/tempo[thisNote];

       buzz(melodyPin, melody[thisNote],noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       int pauseBetweenNotes = noteDuration * 1.30;
       delay(pauseBetweenNotes);

       // stop the tone playing:
       buzz(melodyPin, 0,noteDuration);
       
       // Stop playing when the trigger is LOW
       if (!isTriggerActive(false)) {
         break;
       }
    }
}

void buzz(int targetPin, long frequency, long length) {
  long delayValue = 1000000/frequency/2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length/ 1000; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to 
  //// get the total number of cycles to produce
  for (long i=0; i < numCycles; i++){ // for the calculated length of time...
    digitalWrite(targetPin,HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(targetPin,LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue); // wait again or the calculated delay value
  }
}
