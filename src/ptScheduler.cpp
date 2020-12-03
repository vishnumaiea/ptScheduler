
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blocking
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.9
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:11:47 AM 03-12-2020, Thursday

//=======================================================================//

//  Read the header file for better understanding of how this library is
//  organized. I have added plenty of comments whereever possible.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//constructors

//creates the basic type of task.
//accepts a single interval in milliseconds.

ptScheduler::ptScheduler (time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  intervalList[0] = interval_1;
  interval_s = interval_1;  //save the signed value
  intervalCount = 1;
  intervalIndex = 0;
  activated = true;

  taskMode = PT_MODE1;  //default mode
  sleepMode = PT_SLEEP_MODE1; //task will be deactivated when an iteration completes
}

//----------------------------------------------------------------------//
//same as the previous one but you can also specify the mode.
//all modes with equal intervals will work.

ptScheduler::ptScheduler (uint8_t mode, time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  intervalList[0] = interval_1;
  interval_s = interval_1;
  intervalCount = 1;
  intervalIndex = 0;
  activated = true;
  sleepMode = PT_SLEEP_MODE1;

  switch (taskMode) {
    case PT_MODE1:  //for a single interval, only these modes are supported
    case PT_MODE2:
    case PT_MODE3:
    case PT_MODE4:
      taskMode = mode;
      // inputError = false;
      break;
    
    default:
      taskMode = PT_MODE1;
      inputError = true;
      break;
  }
}

//----------------------------------------------------------------------//
//accepts two intervals. intervals can be same or different.
//works with all mdoes.

ptScheduler::ptScheduler (uint8_t mode, time_ms_t interval_1, time_ms_t interval_2) {
  intervalList = new time_ms_t(2);  //create a new list
  intervalList[0] = interval_1;
  intervalList[1] = abs(interval_2);  //we don't need more than one negative intervals
  interval_s = interval_1;
  intervalCount = 2;
  intervalIndex = 0;
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
      // inputError = false;
      break;
    
    default:
      taskMode = PT_MODE1;
      inputError = true;  //when the user has made a mistake
      break;
  }
}

//----------------------------------------------------------------------//
//this accepts a list of intervals. you have to create an array of intervals
//in the globals scope and pass the pointer to this along with the number
//of intervals in the array.

ptScheduler::ptScheduler (uint8_t mode, time_ms_t* listPtr, uint8_t listLength) {
  if ((listPtr != nullptr) && (listLength != 0)) {
    intervalList = listPtr;
    intervalCount = listLength;

    interval_s = intervalList[0]; //save the first signed value

    for (uint8_t i=1; i < intervalCount; i++) {
      intervalList[i] = abs(intervalList[i]); //remove the sign of all other values
    }
    
    intervalIndex = 0;
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

    else {
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

    // inputError = false;
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
//allows you to chnage modes dynamically.
//returns true is the mode was correct.

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
//sleep applies to iterated tasks. when an iteration (a set of intervals)
//complete the execution, we have two options to prevent the task from running
//further. we can either deactivate the task, which will reset all state
//variables to default and all counters to reset. the interval counter,
//iteration counter etc won't run in deactivated mode. the other method
//is to just suspend the task. a suspended task will return false until it
//is resumed again. the intrval counter will increment in suspend mode.

bool ptScheduler::setSleepMode (uint8_t mode) {
  switch (mode) { //check if the input mode is valid
    case PT_SLEEP_MODE1:  //deactivate mode
    case PT_SLEEP_MODE2:  //suspend mode
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
    case PT_MODE1:  //periodic oneshot
    case PT_MODE2:  //iterated oneshot
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
                    //this is the self-suspend mode.
                    //interval counter run and you can resume the task when you need.
                    suspend();
                  }
                  executionCounter = 0;
                  iterationEnded = true;
                  sleepIntervalCounter = 0;
                  return false;
                }
              }
              executionCounter++;
              return true;
            }
            else {
              sleepIntervalCounter++;
              return false;
            }
          }
        }

        //if cycle has started
        //check if a time cycle has elapsed
        else {
          elapsedTime = millis() - entryTime;

          if (elapsedTime >= intervalList[intervalIndex]) {
            if (intervalIndex < (intervalCount-1)) {
              intervalIndex++;
            }
            else {
              intervalIndex = 0;
            }

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

    case PT_MODE3:  //periodic spanning
    case PT_MODE4:  //iterated spanning
    case PT_MODE5:  //unequal periodic spanning
    case PT_MODE6:  //unequal iterated spanning
      if (activated) {
        //if an execution cycle has not started, yet skip for the time set.
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
              runState = true;
            }
          }
        }   

        //if a cycle has not started yet.
        //for spanning tasks, this only happens once.
        if (!cycleStarted) {
          if (intervalList[0] < 0) {  //if the first interval value is negative
            intervalList[0] = abs(intervalList[0]); //sign is lost here
            runState = false;
            running = false;
            dormant = true;
          }
          else {  //if interval is not negative
            runState = true;
            running = true;
            dormant = false;
            executionCounter++; //this increments before the interval counter
          }
          entryTime = millis();
          cycleStarted = true;
          iterationEnded = false;
          taskStarted = true;
          intervalIndex = 0;
          // printStats();
        }
        else {
          elapsedTime = millis() - entryTime;

          if (elapsedTime >= intervalList[intervalIndex]) {
            if (intervalIndex < (intervalCount-1)) {
              intervalIndex++;
            }
            else {
              intervalIndex = 0;
            }

            intervalCounter++;
            exitTime = entryTime + elapsedTime;
            entryTime = millis();

            if (!suspended) {
              if (iterations > 0) { //if this is an iterated task
                if (executionCounter == iterations) { //when the specified no. of iterations have been reached
                  if (sleepMode == PT_SLEEP_MODE1) {
                    deactivate(); //interval counter will not run in this mode
                  }
                  else {
                    //this is the self-suspend mode.
                    suspend();
                  }
                  //when an iteration has completed
                  // printStats();
                  executionCounter = 0; //so that we can start a new iteration
                  runState = false;
                  running = false;
                  dormant = true;
                  iterationEnded = true;
                  sleepIntervalCounter = 0;
                  // intervalIndex = 0;  //you have to manually reset this, if you want
                  return false;
                }
              }

              //if not an iterated task
              runState = (runState) ? false : true; //toggle the state

              if (runState) {
                executionCounter++;
                running = true;
                dormant = false;
              }
              else {
                running = false;
                dormant = true;
              }
            }
            else {  //if suspended
              // printStats();
              sleepIntervalCounter++; //time elapsed after iteration is complete
              return false;
            }
          }
        }
        return runState;
      }

      else { //if not activated
        return false;
      }

    default:
      break;
  }
  
  return false;
}

//=======================================================================//
//prints all the state variables and counters.

void ptScheduler::printStats() {
  debugSerial.print(F("Interval Count : "));
  debugSerial.println(intervalCount);
  debugSerial.print(F("Intervals (ms) : "));

  for (int i=0; i < intervalCount; i++) {
    debugSerial.print((int32_t) intervalList[i]);

    if (i != (intervalCount-1)) {
      debugSerial.print(F(", "));
    }
    else {
      debugSerial.println();
    }
  }
    
  debugSerial.print(F("Task Mode : "));
  debugSerial.println(taskMode);
  debugSerial.print(F("Sleep Mode : "));
  debugSerial.println(sleepMode);
  debugSerial.print(F("Skip Interval : "));
  debugSerial.println(skipInterval);
  debugSerial.print(F("Skip Iteration : "));
  debugSerial.println(skipIteration);
  debugSerial.print(F("Skip Time : "));
  debugSerial.println((int32_t) skipTime);
  debugSerial.print(F("Iterations : "));
  debugSerial.println(iterations);
  debugSerial.print(F("Entry Time : "));
  debugSerial.println((int32_t) entryTime);
  debugSerial.print(F("Elapsed Time : "));
  debugSerial.println((int32_t) elapsedTime);
  debugSerial.print(F("Exit Time : "));
  debugSerial.println((int32_t) exitTime);
  debugSerial.print(F("Residual Time : "));
  debugSerial.println((int32_t) residualTime);
  debugSerial.print(F("Interval Counter : "));
  debugSerial.println((uint32_t)intervalCounter);
  debugSerial.print(F("Execution Counter : "));
  debugSerial.println((uint32_t)executionCounter);
  debugSerial.print(F("Iteration Counter : "));
  debugSerial.println((uint32_t)iterationCounter);
  debugSerial.print(F("Activated ? : "));
  debugSerial.println(activated);
  debugSerial.print(F("Suspended ? : "));
  debugSerial.println(suspended);
  debugSerial.print(F("Task Started ? : "));
  debugSerial.println(taskStarted);
  debugSerial.print(F("Cycle Started ? : "));
  debugSerial.println(cycleStarted);
  debugSerial.print(F("Dormant ? : "));
  debugSerial.println(dormant);
  debugSerial.print(F("Running ? : "));
  debugSerial.println(running);
  debugSerial.print(F("Ended ? : "));
  debugSerial.println(iterationEnded);
  debugSerial.print(F("Run State : "));
  debugSerial.println(runState);
  debugSerial.print(F("Input Error : "));
  debugSerial.println(inputError);
  debugSerial.print(F("Iterations : "));
  debugSerial.println(iterations);
  debugSerial.println();
}

//=======================================================================//
//activates a task.
//only active tasks are executed regardless of their turn for execution.

void ptScheduler::activate() {
  activated = true;
}

//=======================================================================//
//suspends a task. interval counter will continue to increment in this mode.

void ptScheduler::suspend() {
  suspended = true;
}

//=======================================================================//
//resume a task from suspended state. the task will resume from where it
//was suspended.

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
  iterationEnded = true;
  running = false;

  executionCounter = 0;
  intervalCounter = 0;
  intervalIndex = 0;
  sleepIntervalCounter = 0;
  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  residualTime = 0;
  intervalList[0] = interval_s; //this will be preserved even if the value was negative
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  deactivate();
  activate();
}

//=======================================================================//
//set the iteration number for a task. the iterations are compared to the
//execution counter each time. when the set number of executions has
//completed, the task will go to sleep mode. the sleep mode is user
//definable.

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
      inputError = true;
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
    inputError = true;
    return false;
  }
}

//----------------------------------------------------------------------//

bool ptScheduler::setInterval (time_ms_t value_1, time_ms_t value_2) {
  if (intervalCount > 1) {
    intervalList[0] = value_1;
    intervalList[0] = value_2;
    return true;
  }
  else {
    inputError = true;
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
