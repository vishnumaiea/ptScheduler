
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.3
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 12:17:16 AM 29-11-2020, Sunday

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

//create tasks
ptScheduler sayHello = ptScheduler(1000);
ptScheduler sayName = ptScheduler(-3000);
ptScheduler basicBlink = ptScheduler(1000);
ptScheduler multiBlink = ptScheduler(100);
ptScheduler plot = ptScheduler(50);

ptScheduler epoTask = ptScheduler(1000);  //equal, periodic, oneshot
ptScheduler eioTask = ptScheduler(PT_MODE2, 1000);  //equal, iterated, oneshot
ptScheduler epsTask = ptScheduler(PT_MODE3, 1000);  //equal, periodic, spanning
ptScheduler eisTask = ptScheduler(PT_MODE4, 1000); //equal, iterated, spanning

uint8_t ledOn = false;  //a var to toggle the LED state

//=======================================================================//

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  // Serial.print("\n-- ptScheduler --\n\n");

  // epoTask.setSkipInterval(4);
  // epoTask.setSkipTime(1000);
  // epoTask.setSkipIteration(5);

  // eioTask.setIteration(5);
  // eioTask.setSleepMode(PT_SLEEP_MODE2);

  eisTask.setIteration(5);
  eisTask.setSleepMode(PT_SLEEP_MODE2);
}

//=======================================================================//

void loop() {
  // //executed every second
  // if (sayHello.call()) {
  //   Serial.println("Hello World");
  // }
  
  // //skips first time and executed every 3 seconds
  // if (sayName.call()) {
  //   Serial.println("I am ptScheduler");
  //   multiBlink.activate();
  // }
  
  // //toggles LED every second
  // if (basicBlink.call()) {
  //   digitalWrite (LED2, !digitalRead(LED2));
  // }

  // //periodic indefinite oneshot task
  // if (epoTask.call()) {
  //   Serial.println("3");

  //   if ((epoTask.intervalCounter >= 5) && (epoTask.intervalCounter <= 10)) {
  //     epoTask.suspend();
  //   }
  // }
  // else if (plot.call()) {
  //   Serial.println("0");

  //   if (epoTask.intervalCounter > 10) {
  //     epoTask.resume();
  //   }
  // }

  // //equal interval, iterated and oneshot task
  // if (eioTask.call()) {
  //   Serial.println("3");
  // }
  // else if (plot.call()) {
  //   Serial.println("0");
  //   if (eioTask.intervalCounter == 10) {
  //     eioTask.resume();
  //   }
  // }
  
  // //equal interval, periodic indefinite and spanning task
  // if (plot.call()) {
  //   if (epsTask.call()) {
  //     Serial.println("3");
  //   }
  //   else {
  //     Serial.println("0");
  //     if (epsTask.intervalCounter == 10) {
  //       spanningTask.suspend();
  //     }
  //     else if (epsTask.intervalCounter > 15) {
  //       epsTask.resume();
  //     }
  //   }
  // }

  //equal interval, iterated and spanning task
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

  //task as a function
  blinker();
}

//=======================================================================//
//blinks an LED three times when activated and stops
//activate again to blink

void blinker() {
  if (multiBlink.call()) {
    if (multiBlink.executionCounter == 1) {  //task counter starts at 1
      ledOn = true;
    }
    else {
      ledOn = !ledOn; //toggle state
    }

    if (ledOn) {
      digitalWrite (LED1, HIGH);
    }

    else {
      digitalWrite(LED1, LOW);
    }

    if (multiBlink.executionCounter == 6) {  //it requires 6 intervals for 3 blinks
      multiBlink.deactivate();
      multiBlink.executionCounter = 0; //reset task counter
      digitalWrite(LED1, LOW);  //bring the LED to idle state
    }
  }
}

//=======================================================================//

