
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blocking
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.2
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 12:15:41 PM 14-11-2020, Saturday

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//constructor

ptScheduler::ptScheduler(int64_t timeValue = 0) {
  interval = timeValue;
  interval_ = timeValue;
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
      dormant = true;
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
        dormant = false;
        taskCounter++;
        return true;
      }
      else {
        running = false;
        dormant = true;
        return false;
      }
    }
  }

  else if ((started)) {
    elapsed = millis() - entry;
    if (elapsed >= interval) {
      started = false;
      running = false;
      dormant = false;
      ended = true;
      exit = entry + elapsed;
      return false;
    }
    else {
      // started = true;
      // ended = false;
      running = false;
      dormant = true;
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

void ptScheduler::suspend() {
  activated = false;
}

//=======================================================================//
//deactivates a task.

void ptScheduler::deactivate() {
  activated = false;
  taskCounter = 0;
  intervalCounter = 0;
  interval = interval_;
  entry = 0;
  // exit = 0;
  // elapsed = 0;
  // residue = 0;
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
  interval_ = timeValue;
}

//=======================================================================//
