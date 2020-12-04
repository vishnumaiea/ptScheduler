
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using ordinary NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.9
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:11:37 AM 03-12-2020, Thursday

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
