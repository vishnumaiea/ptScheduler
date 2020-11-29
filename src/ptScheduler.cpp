
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

ptScheduler::ptScheduler (time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  intervalList[0] = interval_1;
  interval_s = interval_1;
  intervalCount = 1;
  activated = true;

  taskMode = PT_MODE1;  //default mode
  sleepMode = PT_SLEEP_MODE1;
}

//=======================================================================//

ptScheduler::ptScheduler (uint8_t mode, time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  intervalList[0] = interval_1;
  interval_s = interval_1;
  intervalCount = 1;
  activated = true;
  sleepMode = PT_SLEEP_MODE1;

  switch (taskMode) {
    case PT_MODE1:  //for a single interval, only these modes are supported
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
      taskMode = mode;
      break;
    
    default:
      taskMode = PT_MODE1;
      break;
  }
}

//=======================================================================//

ptScheduler::ptScheduler (uint8_t mode, time_ms_t interval_1, time_ms_t interval_2) {
  intervalList = new time_ms_t(2);  //create a new list
  intervalList[0] = interval_1;
  intervalList[1] = interval_2;
  interval_s = interval_1;
  intervalCount = 2;
  activated = true;
  sleepMode = PT_SLEEP_MODE1;

  switch (taskMode) {
    case PT_MODE1:
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
    case PT_MODE5:
    case PT_MODE6:
      taskMode = mode;
      break;
    
    default:
      taskMode = PT_MODE1;
      break;
  }
}

//=======================================================================//

ptScheduler::ptScheduler (uint8_t mode, time_ms_t* listPtr, uint8_t listLength) {
  if ((listPtr != nullptr) && (listLength != 0)) {
    intervalList = listPtr;
    intervalCount = listLength;

    interval_s = intervalList[0]; //save the signed value

    for (uint8_t i=1; i < intervalCount; i++) {
      intervalList[i] = abs(intervalList[i]); //remove the sign of all other values
    }

    activated = true;
    sleepMode = PT_SLEEP_MODE1;
    
    //now we have to determine if the specified mode is possible with the input parameters
    if (intervalCount == 1) {
      switch (taskMode) {
        case PT_MODE1:  //for a single interval, only these modes are supported
        case PT_MODE2:
        case PT_MODE3:
        case PT_MODE4:
          taskMode = mode;
          break;
        
        default:
          taskMode = PT_MODE1;
          break;
      }
    }

    else if (intervalCount == 2) {
      switch (taskMode) {
        case PT_MODE1:
        case PT_MODE2:
        case PT_MODE3:
        case PT_MODE4:
        case PT_MODE5:
        case PT_MODE6:
          taskMode = mode;
          break;
        
        default:
          taskMode = PT_MODE1;
          break;
      }
    }

    else {
      taskMode = PT_MODE1;
    }

    inputError = false;
  }
  else {  //if the input parameters are invalid
    ptScheduler(1000);
    inputError = true;
  }
  
}

//=======================================================================//
//destructor

ptScheduler::~ptScheduler() {
  
}

//=======================================================================//

bool ptScheduler::setTaskMode (uint8_t mode) {
  switch (mode) { //check if the input mode is valid
    case PT_MODE1:
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
    case PT_MODE5:
    case PT_MODE6:
      taskMode = mode;
      return true;
      break;
    
    default:
      taskMode = PT_MODE1;
      inputError = true;
      return false;
      break;
  }
}

//=======================================================================//

bool ptScheduler::setSleepMode (uint8_t mode) {
  switch (mode) { //check if the input mode is valid
    case PT_SLEEP_MODE1:
    case PT_SLEEP_MODE2:
      sleepMode = mode;
      return true;
      break;
    
    default:
      sleepMode = PT_SLEEP_MODE1;
      inputError = true;
      return false;
      break;
  }
}

//=======================================================================//
//determines whether a task has to be executed.
//enclose this in a conditional statement to run that block of code.
//do not use other blocking delays.

bool ptScheduler::call() {
  switch (taskMode) {
    case PT_MODE1:
    case PT_MODE2:
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
          if (intervalList[0] < 0) { //if interval is negative,task will be skipped for that time
            intervalList[0] = abs(intervalList[0]); //sign is lost here
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
              if (iterations > 0) { //if this is an iterated task
                if (executionCounter == iterations) { //when the specified no. of iterations have been reached
                  if (sleepMode == PT_SLEEP_MODE1) {
                    deactivate(); //interval counter will not run in this mode
                  }
                  else {
                    executionCounter = 0;
                    //this is the self-suspend mode.
                    //interval counter run and you can resume the task when you need.
                    suspend();  
                  }
                  return false;
                }
              }
              executionCounter++;
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
          if (elapsedTime >= intervalList[0]) {
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

  executionCounter = 0;
  intervalCounter = 0;
  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  residue = 0;
  intervalList[0] = interval_s; //this will be preserved even if the value was negative
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  deactivate();
  activate();
}

//=======================================================================//

bool ptScheduler::setIteration (int32_t value) {
  switch (taskMode) {
    case PT_MODE2:
    case PT_MODE4:
    case PT_MODE6:
      iterations = value;
      return true;
      break;
    
    default:
      iterations = 0;
      return false;
      break;
  }
}

//=======================================================================//
//set the interval at which you want the task to be executed.
//time is in milliseconds.
//a positive value will execute the task at 0th time and then every time
//the set interval has passed. a minus value will postpone the task until
//the set interval has passed for the first time. after, the task is
//executed at every intervals.
//input is time in milliseconds.

bool ptScheduler::setInterval (time_ms_t value) {
  if (intervalCount > 0) {
    intervalList[0] = value;
    return true;
  }
  else {
    return false;
  }
}

//=======================================================================//

bool ptScheduler::setInterval (time_ms_t value_1, time_ms_t value_2) {
  if (intervalCount > 1) {
    intervalList[0] = value_1;
    intervalList[0] = value_2;
    return true;
  }
  else {
    return false;
  }
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

  switch (taskMode) {
    case PT_MODE1:
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
    default:
      skipTime = skipInterval * abs(intervalList[0]);
      skipTimeSet = true;
      return true;

      break;
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

  switch (taskMode) {
    case PT_MODE1:
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
    default:
      skipTime = skipIteration * abs(intervalList[0]);
      skipTimeSet = true;
      return true;

      break;
  }
}

//=======================================================================//
//skips the specified time before each execution cycle.
//for itertated tasks, this is before each iteration.
//for indefinite tasks, this happens at the beginning once, unless you
//deactivate or reset it.
//input is time on milliseconds.

bool ptScheduler::setSkipTime (time_ms_t value) {
  skipTime = abs(value);
  skipTimeSet = true;
  return true;
}

//=======================================================================//
