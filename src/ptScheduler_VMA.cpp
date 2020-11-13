
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blocking
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.1
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 10:48:05 PM 13-11-2020, Friday

//=======================================================================//
//includes

#include "ptScheduler_VMA.h"

//=======================================================================//
//constructor

ptScheduler::ptScheduler(int64_t timeValue = 0) {
  interval = timeValue;
  activated = true;
}

//=======================================================================//
//destructor

ptScheduler::~ptScheduler() {
  
}

//=======================================================================//
//determines whether a task has to be executed.
//enclose this in a conditional statement to run that block of code.
//do not use other blocking delays.

bool ptScheduler::call() {
  if ((!started)) {
    if (interval < 0) {
      interval = abs(interval);
      entry = millis();
      started = true;
      running = false;
      suspended = true;
      ended = false;
      return false;
    }
    else {
      entry = millis();
      started = true;
      ended = false;
      intervalCounter++;

      if (activated) {
        running = true;
        suspended = false;
        taskCounter++;
        return true;
      }
      else {
        running = false;
        suspended = true;
        return false;
      }
    }
  }

  else if ((started)) {
    elapsed = millis() - entry;
    if (elapsed >= interval) {
      started = false;
      running = false;
      suspended = false;
      ended = true;
      exit = entry + elapsed;
      return false;
    }
    else {
      // started = true;
      // ended = false;
      running = false;
      suspended = true;
      return false;
    }
  }
  
  return false;
}

//=======================================================================//
//activates a task.
//only active tasks are executed regardless of their right to execution.

void ptScheduler::activate() {
  activated = true;
}

//=======================================================================//
//deactivates a task.

void ptScheduler::deactivate() {
  activated = false;
}

//=======================================================================//
//set the interval at which you want the task to be executed.
//time is in milliseconds.
//a positive value will execute the task at 0th time and then every time
//the set interval has passed. a minus value will postpone the task until
//the set interval has passed for the first time. after, the task is
//executed at every intervals.

void ptScheduler::setInterval(int64_t timeValue) {
  interval = timeValue;
}

//=======================================================================//
