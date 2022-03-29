
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 20:34:23 PM 27-03-2022, Sunday

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

#define debugSerial Serial

//=======================================================================//
//globals

time_us_t intervalArray[] = {1000000, 2000000, 3000000, 2500000, 4000000}; 
time_us_t intervalArray_2[] = {1000000, 1000000}; 

//create tasks
ptScheduler plot = ptScheduler(50000); //serial plotter task

// ptScheduler oiTask = ptScheduler(PT_MODE_OI, 1000000);
ptScheduler oiTask = ptScheduler(PT_MODE_OI, intervalArray, 3); //oneshot, infinite
ptScheduler siTask = ptScheduler(PT_MODE_SI, intervalArray_2, 2);  //spanning, infinite
ptScheduler sfTask = ptScheduler(1000); //spanning, finite

uint8_t ledOn = false;  //a var to toggle the LED state

//=======================================================================//

void setup() {
  debugSerial.begin(9600);
  while(!debugSerial);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  
  // oiTask.setSkipTime(3000000);

  //setting repetitions to 0 means infinite repetitions.
  // oiTask.setRepetition(0);

  //setting a repetition value to an infinite task will convert it to a finite task.
  oiTask.setRepetition(0);
  siTask.setRepetition(0);

  //when setting a skip paramter after one has already been set,
  //only the last parameter will take effect.
  oiTask.setSkipInterval(0);
  siTask.setSkipInterval(0);

  //start disabled
  oiTask.disable();
  siTask.disable();
}

//=======================================================================//

void loop() {
  // oiFunction();
  siFunction();
}

//=======================================================================//
//oneshot infinte task

void oiFunction() {
  //a set of commands to control the task
  if (debugSerial.available()) {
    String inputString = debugSerial.readString();
    
    if (inputString == "1") {
      oiTask.reset();
    }
    if (inputString == "2") {
      oiTask.suspend();
    }
    if (inputString == "3") {
      oiTask.resume();
    }
    if (inputString == "4") {
      oiTask.disable();
    }
  }
  
  if (plot.call()) {
    if (oiTask.call()) {
      debugSerial.println("3");
      // oiTask.printStats();
    }
    else {
      debugSerial.println("0");
      // if (oiTask.repetitionsEnded) {
      //   // oiTask.printStats();
      //   oiTask.repetitionsEnded = false;
      // }
    }
  }
}

//=======================================================================//
//spaning infinite task

void siFunction() {
  //a set of commands to control the task
  if (debugSerial.available()) {
    String inputString = debugSerial.readString();
    
    if (inputString == "1") {
      siTask.reset();
    }
    if (inputString == "2") {
      siTask.suspend();
    }
    if (inputString == "3") {
      siTask.resume();
    }
    if (inputString == "4") {
      siTask.disable();
    }
  }
  
  if (plot.call()) {
    if (siTask.call()) {
      debugSerial.println("3");
      // siTask.printStats();
    }
    else {
      debugSerial.println("0");
      // if (siTask.repetitionsEnded) {
      //   // siTask.printStats();
      //   siTask.repetitionsEnded = false;
      // }
    }
  }
}

// //=======================================================================//
// //unequal interval, periodic and spanning task

// void upsFunction() {
//   if (plot.call()) {
//     if (upsTask.call()) {
//       Serial.println("3");
//     }
//     else {
//       Serial.println("0");
//     }
//   }
// }

// //=======================================================================//
// //equal interval, iterated and spanning task

// void eisFunction() {
//   if (plot.call()) {
//     if (eisTask.call()) {
//       Serial.println("3");
//     }
//     else {
//       Serial.println("0");
//       if (eisTask.sleepIntervalCounter == 10) {
//         eisTask.resume();
//       }
//     }
//   }
// }

// //=======================================================================//
// //equal interval, periodic and spanning task

// void epsFunction() {
//   if (plot.call()) {
//     if (epsTask.call()) {
//       Serial.println("3");
//     }
//     else {
//       Serial.println("0");
//       if (epsTask.intervalCounter == 10) {
//         epsTask.suspend();
//       }
//       else if (epsTask.intervalCounter > 15) {
//         epsTask.resume();
//       }
//     }
//   }
// }

// //=======================================================================//
// //equal interval, iterated and oneshot task

// void eioFunction() {
//   if (eioTask.call()) {
//     Serial.println("3");
//   }
//   else if (plot.call()) {
//     Serial.println("0");
//     // if (eioTask.intervalCounter == 5) {
//     //   eioTask.resume();
//     // }
//     // if (eioTask.intervalCounter == 10) {
//     //   eioTask.resume();
//     // }
//     // if (eioTask.intervalCounter == 12) {
//     //   eioTask.resume();
//     // }
//   }
// }

// //=======================================================================//
// //equal interval, periodic and oneshot task

// void epoFunction() {
//   if (epoTask.call()) {
//     Serial.println("3");

//     // if ((epoTask.intervalCounter >= 5) && (epoTask.intervalCounter <= 10)) {
//     //   epoTask.suspend();
//     // }
//   }
//   else if (plot.call()) {
//     Serial.println("0");

//     // if (epoTask.intervalCounter > 10) {
//     //   epoTask.resume();
//     // }
//   }
// }

// //=======================================================================//

