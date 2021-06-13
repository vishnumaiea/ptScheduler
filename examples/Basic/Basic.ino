
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 1.1.2
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 11:58:55 AM 13-06-2021, Sunday

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

#include "ptScheduler.h"

//=======================================================================//
//defines

#define LED1 LED_BUILTIN
#define LED2 12

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello = ptScheduler(1000);
ptScheduler sayName = ptScheduler(3000);
ptScheduler basicBlink = ptScheduler(1000);
ptScheduler multiBlink = ptScheduler(PT_MODE_EIO, 100); //Equal, Iterated, Oneshot

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  multiBlink.setSleepMode(PT_SLEEP_SUSPEND);  //suspend mode so that we can resume the task periodically
  multiBlink.setIteration(6); //for three blinks, we need 6 iterations (3 OFF states, and 3 ON states)

  Serial.print("\n-- ptScheduler --\n\n");
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
    digitalWrite (LED2, !digitalRead(LED2));
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
  else if (multiBlink.sleepIntervalCounter >= 20) { //100 ms * 20 = 2000 ms
    multiBlink.resume();  //resume the suspended task so that LED will blink again
  }
}

//=======================================================================//

