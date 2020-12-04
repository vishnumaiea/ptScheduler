
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using ordinary NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.9
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:11:42 AM 03-12-2020, Thursday

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

time_ms_t intervalArray[] = {2000, 1000, 3000}; 

//create tasks
ptScheduler plot = ptScheduler(50); //serial plotter task

ptScheduler epoTask = ptScheduler(1000);  //equal, periodic, oneshot
ptScheduler eioTask = ptScheduler(PT_MODE2, 1000);  //equal, iterated, oneshot
ptScheduler epsTask = ptScheduler(PT_MODE3, 1000);  //equal, periodic, spanning
ptScheduler eisTask = ptScheduler(PT_MODE4, 1000); //equal, iterated, spanning
ptScheduler upsTask = ptScheduler(PT_MODE5, 1000, 2000);  //unequal, periodic, spanning
ptScheduler uisTask = ptScheduler(PT_MODE6, intervalArray, 3);  //unequal, iterated, spanning

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
  eioTask.setSleepMode(PT_SLEEP_MODE2);

  eisTask.setIteration(5);
  eisTask.setSleepMode(PT_SLEEP_MODE2);

  uisTask.setIteration(5);
  uisTask.setSleepMode(PT_SLEEP_MODE2);
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

