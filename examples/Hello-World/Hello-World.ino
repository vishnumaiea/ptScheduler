
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 1.1.2
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 11:58:59 AM 13-06-2021, Sunday

//=======================================================================//
//description

//  Prints "Hello World" to the serial monitor every second.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//globals

//create tasks
ptScheduler sayHello(1000); //Equal, Periodic, Oneshot

//=======================================================================//
//setup function runs once

void setup() {
  Serial.begin(9600);
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
