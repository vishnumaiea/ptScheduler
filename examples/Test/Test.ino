
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.1.1
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 20:09:21 PM 03-12-2022, Saturday

//=======================================================================//
//description

//This is the sketch I use to test the library.

//=======================================================================//
//includes

#include <ptScheduler.h>

//=======================================================================//
//defines

#define LED1 LED_BUILTIN
#define debugSerial Serial

//=======================================================================//
//globals

time_us_t intervalArray [] = {1000000, 2000000, 3000000, 2500000, 4000000}; 

// create tasks
ptScheduler plot = ptScheduler (PT_FREQ_20HZ); //serial plotter task

// ptScheduler oneshotTask = ptScheduler(PT_MODE_OI, 1000000);
ptScheduler oneshotTask = ptScheduler (PT_MODE_ONESHOT, intervalArray, 1); //oneshot, infinite
ptScheduler spanningTask = ptScheduler (PT_MODE_SPANNING, intervalArray, 2);  //spanning, infinite

uint8_t ledOn = false;  //a var to toggle the LED state

//=======================================================================//
// forward declarations

void oneshotFunction();
void spanningFunction();

//=======================================================================//

void setup() {
  debugSerial.begin (9600);
  // while (!debugSerial);
  
  pinMode (LED1, OUTPUT);
  
  // oneshotTask.setSkipTime (3000000);

  // setting a repetition value to an infinite task will convert it to a finite task.
  oneshotTask.setSequenceRepetition (0);
  spanningTask.setSequenceRepetition (2);

  // when setting a skip paramter after one has already been set,
  // only the last parameter will take effect.
  oneshotTask.setSkipInterval (0);
  spanningTask.setSkipInterval (0);

  // spanningTask.setSleepMode (PT_SLEEP_DISABLE);
  spanningTask.setSleepMode (PT_SLEEP_SUSPEND);

  // start disabled
  oneshotTask.suspend();
  spanningTask.disable();

  // delay (3000);
  // oneshotTask.reset();
  // spanningTask.reset();
}

//=======================================================================//

void loop() {
  // oneshotFunction();
  spanningFunction();
}

//=======================================================================//
// oneshot infinte task

void oneshotFunction() {
  // a set of commands to control the task
  if (debugSerial.available()) {
    String inputString = debugSerial.readString();
    
    if (inputString == "1") {
      oneshotTask.reset();
    }
    if (inputString == "2") {
      oneshotTask.suspend();
    }
    if (inputString == "3") {
      oneshotTask.resume();
    }
    if (inputString == "4") {
      oneshotTask.disable();
    }
    if (inputString == "5") {
      oneshotTask.enable();
    }
  }
  
  if (plot.call()) {
    if (oneshotTask.call()) {
      debugSerial.println ("3");
    }
    else {
      debugSerial.println ("0");
    }
  }
}

//=======================================================================//
//spaning infinite task

void spanningFunction() {
  //a set of commands to control the task
  if (debugSerial.available()) {
    String inputString = debugSerial.readString();
    
    if (inputString == "1") {
      spanningTask.reset();
    }
    if (inputString == "2") {
      spanningTask.suspend();
    }
    if (inputString == "3") {
      spanningTask.resume();
    }
    if (inputString == "4") {
      spanningTask.disable();
    }
    if (inputString == "5") {
      spanningTask.enable();
    }
  }
  
  if (plot.call()) {
    if (spanningTask.call()) {
      debugSerial.println ("3");
      digitalWrite (LED1, LOW);
    }
    else {
      debugSerial.println ("0");
      digitalWrite (LED1, HIGH);
    }
  }
}

//=======================================================================//
