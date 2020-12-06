
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 1.1.0
// License : MIT
// Src : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:20:18 PM 06-12-2020, Sunday

//=======================================================================//
//description

//  Prints "Hello World" to the serial monitor every second.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello = ptScheduler(1000);

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(9600);
}

//=======================================================================//
//infinite loop

void loop() {
  //executed every second
  if (sayHello.call()) {
    Serial.println("Hello World");
  }

  //add other tasks and non-blocking code here
}

//=======================================================================//
