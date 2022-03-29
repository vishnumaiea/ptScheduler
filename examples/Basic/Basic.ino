
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.1.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 23:27:23 PM 29-03-2022, Tuesday

//=======================================================================//
//description

//This is a basic usage example of ptScheduler. It creates four tasks.
//Two tasks, sayHello and sayName simply print to the serial monitor at
//specified intervals. The other two tasks, basicBlink and multiBlink
//blinks two LEDs. The basicBlink blinks an LED every second indefinitely.
//The multiBlink blinks the LED for three times in a burst manner. multiBlink
//is an example of an iterated task that stops after a preset number of
//iterations.

//=======================================================================//
//includes

#include <ptScheduler.h>

//=======================================================================//
//defines

#define LED1 LED_BUILTIN
#define LED2 2

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello = ptScheduler(PT_FREQ_1HZ);
ptScheduler sayName = ptScheduler(PT_FREQ_5HZ);
ptScheduler basicBlink = ptScheduler(PT_TIME_1S);
ptScheduler multiBlink = ptScheduler(PT_MODE_ONESHOT, PT_TIME_100MS);

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  multiBlink.setSleepMode(PT_SLEEP_SUSPEND);  //suspend mode so that we can resume the task periodically
  multiBlink.setSequenceRepetition(6); //for three blinks, we need 6 iterations (3 OFF states, and 3 ON states)

  Serial.print("\n-- ptScheduler --\n\n");
  digitalWrite(LED1, HIGH);
}

//=======================================================================//
//infinite loop

void loop() {
  //executed every second
  if (sayHello.call()) {
    Serial.println("Hello World");
  }
  
  //skips first time and executed every 3 seconds
  if (sayName.call()) {
    Serial.println("I am ptScheduler");
    multiBlink.enable();
  }
  
  //toggles LED every second
  if (basicBlink.call()) {
    digitalWrite (LED1, !digitalRead(LED1));
  }
  
  //task as a function
  multiBlinker();
}

//=======================================================================//
//blinks an LED three times, every 2 seconds

void multiBlinker() {
  if (multiBlink.call()) {  //returns true every 100 ms
    digitalWrite (LED1, !digitalRead(LED1)); //toggle LED
  }
  //because the sleep mode is SUSPEND, the sleepIntervalCounter still increments.
  //but if the mode was DISABLE, it wouldn't increment.
  else if (multiBlink.suspendedIntervalCounter >= 20) { //100 ms * 20 = 2000 ms
    multiBlink.reset();  //reset the suspended task so that LED will blink again
  }
}

//=======================================================================//

