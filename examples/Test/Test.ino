
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 1.1.0
// License : MIT
// Src : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:20:14 PM 06-12-2020, Sunday

//=======================================================================//
//description

//This is the sketch I use to test the library.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//defines

#define LED1 LED_BUILTIN
#define LED2 12

//=======================================================================//
//globals

time_ms_t intervalArray[] = {1000, 1000, 2000, 2500, 3000}; 

//create tasks
ptScheduler plot = ptScheduler(50); //serial plotter task

ptScheduler epoTask = ptScheduler(1000);  //equal, periodic, oneshot
ptScheduler eioTask = ptScheduler(PT_MODE_EIO, 1000);  //equal, iterated, oneshot
ptScheduler epsTask = ptScheduler(PT_MODE_EPS, 1000);  //equal, periodic, spanning
ptScheduler eisTask = ptScheduler(PT_MODE_EIS, 1000); //equal, iterated, spanning
ptScheduler upsTask = ptScheduler(PT_MODE_UPS, 1000, 2000);  //unequal, periodic, spanning
ptScheduler uisTask = ptScheduler(PT_MODE_UIS, intervalArray, 2);  //unequal, iterated, spanning

uint8_t ledOn = false;  //a var to toggle the LED state

//=======================================================================//

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Serial.print("\n-- ptScheduler --\n\n");

  epoTask.setSkipInterval(4);
  epoTask.setSkipTime(1000);
  epoTask.setSkipIteration(5);

  eioTask.setIteration(5);
  eioTask.setSleepMode(PT_SLEEP_SUSPEND);

  eisTask.setIteration(5);
  eisTask.setSleepMode(PT_SLEEP_SUSPEND);

  uisTask.setIteration(3);
  uisTask.setSleepMode(PT_SLEEP_SUSPEND);
}

//=======================================================================//

void loop() {
  uisFunction();
}

//=======================================================================//
//unequal interval, iterated and spanning task

void uisFunction() {
  if (plot.call()) {
    if (uisTask.call()) {
      Serial.println("3");
    }
    else {
      Serial.println("0");
    }
  }
}

//=======================================================================//
//unequal interval, periodic and spanning task

void upsFunction() {
  if (plot.call()) {
    if (upsTask.call()) {
      Serial.println("3");
    }
    else {
      Serial.println("0");
    }
  }
}

//=======================================================================//
//equal interval, iterated and spanning task

void eisFunction() {
  if (plot.call()) {
    if (eisTask.call()) {
      Serial.println("3");
    }
    else {
      Serial.println("0");
      if (eisTask.sleepIntervalCounter == 10) {
        eisTask.resume();
      }
    }
  }
}

//=======================================================================//
//equal interval, periodic and spanning task

void epsFunction() {
  if (plot.call()) {
    if (epsTask.call()) {
      Serial.println("3");
    }
    else {
      Serial.println("0");
      if (epsTask.intervalCounter == 10) {
        epsTask.suspend();
      }
      else if (epsTask.intervalCounter > 15) {
        epsTask.resume();
      }
    }
  }
}

//=======================================================================//
//equal interval, iterated and oneshot task

void eioFunction() {
  if (eioTask.call()) {
    Serial.println("3");
  }
  else if (plot.call()) {
    Serial.println("0");
    if (eioTask.intervalCounter == 10) {
      eioTask.resume();
    }
  }
}

//=======================================================================//
//equal interval, periodic and oneshot task

void epoFunction() {
  if (epoTask.call()) {
    Serial.println("3");

    if ((epoTask.intervalCounter >= 5) && (epoTask.intervalCounter <= 10)) {
      epoTask.suspend();
    }
  }
  else if (plot.call()) {
    Serial.println("0");

    if (epoTask.intervalCounter > 10) {
      epoTask.resume();
    }
  }
}

//=======================================================================//

