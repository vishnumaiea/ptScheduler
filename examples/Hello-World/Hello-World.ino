
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 17:30:18 PM 27-09-2021, Monday

//=======================================================================//
//description

//  Prints "Hello World" to the serial monitor every second.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello(1000000); //Equal, Periodic, Oneshot

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(115200);
}

//=======================================================================//
//infinite loop

void loop() {
  if (sayHello.call()) {  //executed every second
    Serial.println("Hello World");
  }

  //add other tasks and non-blocking code here
}

//=======================================================================//
