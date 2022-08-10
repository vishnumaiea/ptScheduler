
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.1.1
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 22:42:39 PM 10-08-2022, Wednesday

//=======================================================================//
//description

//  Prints "Hello World" to the serial monitor every second.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello(PT_FREQ_1HZ);

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(9600);
}

//=======================================================================//
//infinite loop

void loop() {
  if (sayHello.call()) {
    Serial.println("Hello World");  //prints every second
  }

  //add other tasks and non-blocking code here
}

//=======================================================================//
