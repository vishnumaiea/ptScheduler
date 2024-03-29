
//=======================================================================//
/**
 * @file Hello-World.ino
 * @author Vishnu Mohanan (@vishnumaiea)
 * @brief "Pretty tiny Scheduler" is an Arduino library for writing non-blocking
 * periodic tasks without using delay() or millis() routines.
 * 
 * Prints "Hello World" to the serial monitor every second.
 * 
 * @version 2.2.0
 * @link https://github.com/vishnumaiea/ptScheduler
 * @date Last modified : +05:30 09:53:45 AM 27-06-2023, Tuesday
 * @copyright License: MIT
 * 
 */
//=======================================================================//
// Includes

#include <ptScheduler.h>

//=======================================================================//
// Globals

// Create the tasks
ptScheduler sayHello (30 * PT_TIME_1S); // Oneshot task

//=======================================================================//
/**
 * @brief setup function runs once.
 * 
 */
void setup() {
  Serial.begin (9600);
}

//=======================================================================//
/**
 * @brief loop function runs repeatedly.
 * 
 */
void loop() {
  if (sayHello.call()) {
    Serial.println ("Hello World");  // prints every second
  }

  // add other tasks and non-blocking code here
}

//=======================================================================//
