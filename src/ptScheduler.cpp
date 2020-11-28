
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blocking
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.3
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 12:18:04 AM 29-11-2020, Sunday

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//constructor

ptScheduler::ptScheduler(int64_t timeValue = 0) {
  interval = timeValue;
  _interval = timeValue;
  activated = true;
  mode = PT_MODE1;
}

//=======================================================================//
//destructor

ptScheduler::~ptScheduler() {
  
}

//=======================================================================//

void ptScheduler::setMode(uint8_t _mode) {
  mode = _mode;
}

//=======================================================================//
//determines whether a task has to be executed.
//enclose this in a conditional statement to run that block of code.
//do not use other blocking delays.

bool ptScheduler::call() {
  switch (mode) {
    case PT_MODE1:
      if (activated) {
        //if an execution cycle has not started, yet skip for the time set
        if ((!taskStarted) && (skipIntervalSet || skipIntervalSet || skipTimeSet)) {
          if (entryTime == 0) { //this is one way to find if an exe cycle not started
            entryTime = millis();
            return false;
          }
          else {
            elapsedTime = millis() - entryTime;

            if (elapsedTime < skipTime) { //skip time is set when skip time or skip interval are set
              return false;
            }
            else {
              taskStarted = true;
            }
          }
        }

        //start a new time cycle
        if (!cycleStarted) { //if a time cycle has not started
          if (interval < 0) { //if interval is negative,task will be skipped for that time
            interval = abs(interval); //sign is lost here
            entryTime = millis();
            cycleStarted = true; //time cycle started
            taskStarted = true; //execution cycle started
            return false;
          }
          else {
            entryTime = millis();
            cycleStarted = true;
            intervalCounter++;  //interval counter increments even if the task is suspended

            //this is the return point that gives you green signal each time 
            if (!suspended) {
              taskCounter++;
              return true;
            }
            else {
              return false;
            }
          }
        }

        //check if a time cycle has elapsed
        else if (cycleStarted) {
          elapsedTime = millis() - entryTime;
          if (elapsedTime >= interval) {
            cycleStarted = false; //so that we can start a new time cycle
            exitTime = entryTime + elapsedTime;
            return false;
          }
          else {
            return false;
          }
        }
      }

      else {
        return false; //if not activated
      }
      
      break;
    
    default:
      break;
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

void ptScheduler::suspend() {
  suspended = true;
}

//=======================================================================//

void ptScheduler::resume() {
  suspended = false;
}

//=======================================================================//
//deactivates a task.
//this will reset all values to their idle state.
//all the user specified values will be preserved.
//this include, mode, skip interval, time, iteration, default interval etc.
//you have to call either activate() or reset() functions to start execution again.

void ptScheduler::deactivate() {
  activated = false;
  taskStarted = false;
  cycleStarted = false;
  dormant = false;
  suspended = false;
  ended = true;
  running = false;

  taskCounter = 0;
  intervalCounter = 0;
  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  residue = 0;
  interval = _interval; //this will be preserved even if the value was negative
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  deactivate();
  activate();
}
//=======================================================================//
//set the interval at which you want the task to be executed.
//time is in milliseconds.
//a positive value will execute the task at 0th time and then every time
//the set interval has passed. a minus value will postpone the task until
//the set interval has passed for the first time. after, the task is
//executed at every intervals.
//input is time in milliseconds.

void ptScheduler::setInterval (int64_t timeValue) {
  interval = timeValue;
  _interval = timeValue;
}

//=======================================================================//
//skips the specified many intervals at the beginning.
//this is different from skipping iterations.
//if you have unequal intervals set, each of them are picked in order,
//until it is equal to the intervals to set.
//skip time is calculated from this selected many intervals by adding them.
//if you set both skip interval and skip iteration, the last one determines
//skip time. input is a positive integer.

bool ptScheduler::setSkipInterval (uint32_t value) {
  skipInterval = value;
  skipIntervalSet = true;

  if (mode == PT_MODE1) {
    skipTime = skipInterval * abs(interval);
    skipTimeSet = true;
    return true;
  }
}

//=======================================================================//
//skips the specified many iterations at the beginning.
//an iteration is the set of all the specified intervals.
//eg. if you have specified 3 unequal intervals, they together form an iteration.
//for periodic oneshot (mode 1) tasks, and tasks with equal intervals
//this is same as setting skip intervals.
//when you set a skip iteration, the skip time is automatically calculated.
//if you have unequal intervals,each set is summed and multiplied by the
//itertations to skip, to find the time.
//input is a positive integer.

bool ptScheduler::setSkipIteration (uint32_t value) {
  skipIteration = value;
  skipIterationSet = true;

  if (mode == PT_MODE1) {
    skipTime = skipIteration * abs(interval);
    skipTimeSet = true;
    return true;
  }
}

//=======================================================================//
//skips the specified time before each execution cycle.
//for itertated tasks, this is before each iteration.
//for indefinite tasks, this happens at the beginning once, unless you
//deactivate or reset it.
//input is time on milliseconds.

bool ptScheduler::setSkipTime (int64_t timeValue) {
  skipTime = abs(timeValue);
  skipTimeSet = true;
  return true;
}

//=======================================================================//
