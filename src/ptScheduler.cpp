
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 18:21:59 PM 14-06-2021, Monday

//=======================================================================//

//  Read the header file for a better understanding of how this library is
//  organized. I have added plenty of comments to help you understand it.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//constructors

//creates the basic type of task.
//accepts a single interval in milliseconds.

ptScheduler::ptScheduler (time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  // intervalList = (time_ms_t*) malloc(sizeof(time_ms_t));  //create a new list
  intervalList[0] = interval_1;
  intervalCount = 1;
  // intervalIndex = 0;
  // taskEnabled = true;

  taskMode = PT_MODE_EPO;  //default mode
  // sleepMode = PT_SLEEP_DISABLE; //task will be disabled when an iteration completes
}

//----------------------------------------------------------------------//
//same as the previous one but you can also specify the mode.
//all modes with equal intervals will work.

ptScheduler::ptScheduler (uint8_t mode, time_ms_t interval_1) {
  intervalList = new time_ms_t(1);  //create a new list
  intervalList[0] = interval_1;
  intervalCount = 1;
  intervalIndex = 0;
  taskEnabled = true;
  sleepMode = PT_SLEEP_DISABLE;

  switch (mode) {
    case PT_MODE_EPO:  //for a single interval, only these modes are supported
    case PT_MODE_EIO:
    case PT_MODE_EPS:
    case PT_MODE_EIS:
      taskMode = mode;
      // inputError = false;
      break;
    
    default:
      taskMode = PT_MODE_EPO;
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
  intervalList[1] = interval_2;
  intervalCount = 2;
  intervalIndex = 0;
  taskEnabled = true;
  sleepMode = PT_SLEEP_DISABLE;

  switch (mode) {
    case PT_MODE_EPO:
    case PT_MODE_EIO:
    case PT_MODE_UPO:
    case PT_MODE_UIO:
    case PT_MODE_EPS:
    case PT_MODE_EIS:
    case PT_MODE_UPS:
    case PT_MODE_UIS:
      taskMode = mode;
      // inputError = false;
      break;
    
    default:
      taskMode = PT_MODE_EPO;
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
    
    intervalIndex = 0;
    taskEnabled = true;
    sleepMode = PT_SLEEP_DISABLE;
    
    //now we have to determine if the specified mode is possible with the input parameters
    if (intervalCount == 1) {
      switch (mode) {
        case PT_MODE_EPO:  //for a single interval, only these modes are supported
        case PT_MODE_EIO:
        case PT_MODE_EPS:
        case PT_MODE_EIS:
          taskMode = mode;
          // inputError = false;
          break;
        
        default:
          taskMode = PT_MODE_EPO;
          inputError = true;
          break;
      }
    }

    else {
      switch (mode) {
        case PT_MODE_EPO:
        case PT_MODE_EIO:
        case PT_MODE_UPO:
        case PT_MODE_UIO:
        case PT_MODE_EPS:
        case PT_MODE_EIS:
        case PT_MODE_UPS:
        case PT_MODE_UIS:
          taskMode = mode;
          // inputError = false;
          break;
        
        default:
          taskMode = PT_MODE_EPO;
          inputError = true;  //when the user has made a mistake
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
//you wouldn't beleive me if I said it took me a whole day to figure out
//how to implement this logic lol.

inline void ptScheduler::timeElapsed() {
  uint32_t timeDelta = uint32_t(micros() - entryTime);
  elapsedTime += uint32_t(timeDelta - prevTimeDelta); //this will always return the absolute difference
  prevTimeDelta = timeDelta;
}

//=======================================================================//
//allows you to change modes dynamically.
//returns true if the mode was correct.

bool ptScheduler::setTaskMode (uint8_t mode) {
  switch (mode) { //check if the input mode is valid
    case PT_MODE_EPO:
    case PT_MODE_EIO:
    case PT_MODE_UPO:
    case PT_MODE_UIO:
    case PT_MODE_EPS:
    case PT_MODE_EIS:
    case PT_MODE_UPS:
    case PT_MODE_UIS:
      taskMode = mode;
      return true;
      break;
    
    default:
      taskMode = PT_MODE_EPO;
      inputError = true;
      return false;
      break;
  }
}

//=======================================================================//
//sleep applies to iterated tasks. when an iteration (a set of intervals)
//complete the execution, we have two options to prevent the task from running
//further. we can either disable the task, which will reset all state
//variables to default and all counters to reset. the interval counter,
//iteration counter etc won't run in disabled mode. the other method
//is to just suspend the task. a suspended task will return false until it
//is resumed again. the intrval counter will increment in suspend mode.

bool ptScheduler::setSleepMode (uint8_t mode) {
  switch (mode) { //check if the input mode is valid
    case PT_SLEEP_DISABLE:  //disable mode
    case PT_SLEEP_SUSPEND:  //suspend mode
      sleepMode = mode;
      return true;
      break;
    
    default:
      sleepMode = PT_SLEEP_DISABLE;
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
    case PT_MODE_EPO: //oneshot tasks
    case PT_MODE_EIO:
    case PT_MODE_UPO:
    case PT_MODE_UIO:
      return oneshot();
      
      break;

    case PT_MODE_EPS: //spanning tasks
    case PT_MODE_EIS:
    case PT_MODE_UPS:
    case PT_MODE_UIS:
      return spanning();

      break;

    default:
      break;
  }
  
  return false;
}

//=======================================================================//
//implements the spanning task logic.

bool ptScheduler::spanning() {
  if (taskEnabled) {
    //if an execution cycle has not started, yet skip for the time set.
    if ((!taskStarted) && (skipIntervalSet || skipIterationSet || skipTimeSet)) {
      if (entryTime == 0) { //this is one way to find if an exe cycle not started
        elapsedTime = 0;
        entryTime = micros();
        return false;
      }
      else {
        elapsedTime = micros() - entryTime;

        if (elapsedTime < skipTime) { //skip time is set when skip time or skip interval are set
          return false;
        }
        else {
          taskStarted = true;
          taskRunState = true;
        }
      }
    }   

    //if an interval cycle has not started yet.
    //for spanning tasks, this only happens once.
    if (!cycleStarted) {
      taskRunState = true;
      taskRunning = true;
      executionCounter++; //this increments before the interval counter
      elapsedTime = 0;  //reset so that we can start a new cycle
      entryTime = micros();
      cycleStarted = true;
      iterationEnded = false;
      taskStarted = true;
      intervalIndex = 0;
      intervalCounter = 0;
      // printStats();
    }
    else {  //if an interval cycle has started
      // elapsedTime = millis() - entryTime;

      //below three lines is a method to circumvent the millis() overflow event.
      //the default Arduino implementation of millis returns a uint32_t value.
      //this value overflows once it reaches 4294967295. this means, the maximum
      //time difference we can have between the entry point and exit point is
      //4294967296 milliseconds. any intervals larger than that would not work.
      //you might not use such a large interval, but it is a limitation nevertheless.
      //this limitation can become a problem if you use micros function. with micros
      //function, the largest interval we can detect is around 70 mins. that might fall
      //within a practical interval value. therefore we must circumvent that limitation.
      //to do this, every time this function is invoked, we will take the difference
      //between the current elapsed time (millis()-entryTime) and the previous known
      //elapsed time. this is to get the absolute time difference from the cumulative
      //time difference (every time we do millis()-entryTime, we get the cumulative
      //time difference). the absolute difference is added to 64-bit elapsedTime.
      // uint32_t timeDelta = uint32_t(millis() - entryTime);
      // elapsedTime += uint32_t(timeDelta - prevTimeDelta); //this will always return the absolute difference
      // prevTimeDelta = timeDelta;
      timeElapsed();  //get the elapsed time

      if (elapsedTime >= intervalList[intervalIndex]) {
        if (intervalIndex < (intervalCount-1)) {
          intervalIndex++;
        }
        else {
          intervalIndex = 0;
        }

        intervalCounter++;
        exitTime = entryTime + elapsedTime;
        entryTime = micros();

        if (!taskSuspended) {
          if (iterations > 0) { //if this is an iterated task
            if (executionCounter == iterationsExtended) { //when the specified no. of iterations have been reached
              if (sleepMode == PT_SLEEP_DISABLE) {
                disable(); //interval counter will not run in this mode
              }
              else {
                //this is the self-suspend mode.
                suspend();
              }
              //when an iteration has completed
              // printStats();

              //when a task suspends itself, it also resets the execution counter.
              //because of this, when you resume a suspended task the next time,
              //the number of iterations are executed once again.
              executionCounter = 0; //so that we can start a new iteration
              
              // intervalCounter = 0;
              taskRunState = false;
              taskRunning = false;
              iterationEnded = true;
              sleepIntervalCounter = 0;
              // intervalIndex = 0;  //you have to manually reset this, if you want
              return false;
            }
          }

          //if not an iterated task
          taskRunState = (taskRunState) ? false : true; //toggle the state

          if (taskRunState) {
            executionCounter++;
            taskRunning = true;
          }
          else {
            taskRunning = false;
          }
        }
        else {  //if suspended
          // printStats();
          sleepIntervalCounter++; //time elapsed after iteration is complete
          return false;
        }
      }
    }
    return taskRunState;
  }

  else { //if not activated
    return false;
  }
} 

//=======================================================================//
//implements the logic of oneshot tasks.

bool ptScheduler::oneshot() {
  if (taskEnabled) {
    //if an execution cycle has not started, yet skip for the time set
    if ((!taskStarted)) {
      if (skipIntervalSet || skipIterationSet || skipTimeSet) {
        if (entryTime == 0) { //this is one way to find if an exe cycle not started
          elapsedTime = 0;
          entryTime = micros();
          return false;
        }
        else {
          // elapsedTime = millis() - entryTime;

          if ((micros() - entryTime) < skipTime) { //skip time is set when skip time or skip interval are set
            return false;
          }
          else {
            taskStarted = true;
          }
        }
      }
    }

    //start a new time cycle
    if (!cycleStarted) { //if a time cycle has not started
      elapsedTime = 0;
      prevTimeDelta = 0;
      entryTime = micros();
      cycleStarted = true;
      intervalCounter++;  //interval counter increments even if the task is suspended

      //this is the return point that gives you green signal each time 
      if (!taskSuspended) {
        if (iterations > 0) { //if this is an iterated task
          if (executionCounter == iterations) { //when the specified no. of iterations have been reached
            if (sleepMode == PT_SLEEP_DISABLE) {
              disable(); //interval counter will not run in this mode
            }
            else {
              //this is the self-suspend mode.
              //interval counter run and you can resume the task when you need.
              suspend();
            }
            //when a task suspends itself, it also resets the execution counter.
            //because of this, when you resume a suspended task the next time,
            //the number of iterations are executed once again. a side effect of this
            //is that, when you suspend a task before an iteration is completed, this
            //will cause the task to start a new iteration when you resume it next time.
            //you may want the actual iteration to resume. if that's the requirement,
            //save the execution counter value before suspending and reload it before
            //resuming.
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

    //if cycle has started
    //check if a time cycle has elapsed
    else {
      // elapsedTime = millis() - entryTime; //this already takes care of millis() overflow
      timeElapsed();

      if (elapsedTime >= intervalList[intervalIndex]) {
        if (intervalIndex < (intervalCount-1)) {
          intervalIndex++;
        }
        else {
          intervalIndex = 0;
        }

        cycleStarted = false; //so that we can start a new time cycle
        // exitTime = entryTime + elapsedTime;
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
  debugSerial.print(F("Interval Counter : "));
  debugSerial.println((uint32_t)intervalCounter);
  debugSerial.print(F("Execution Counter : "));
  debugSerial.println((uint32_t)executionCounter);
  debugSerial.print(F("Iteration Counter : "));
  debugSerial.println((uint32_t)iterationCounter);
  debugSerial.print(F("Enabled ? : "));
  debugSerial.println(taskEnabled);
  debugSerial.print(F("Suspended ? : "));
  debugSerial.println(taskSuspended);
  debugSerial.print(F("Task Started ? : "));
  debugSerial.println(taskStarted);
  debugSerial.print(F("Cycle Started ? : "));
  debugSerial.println(cycleStarted);
  debugSerial.print(F("Running ? : "));
  debugSerial.println(taskRunning);
  debugSerial.print(F("Ended ? : "));
  debugSerial.println(iterationEnded);
  debugSerial.print(F("Run State : "));
  debugSerial.println(taskRunState);
  debugSerial.print(F("Input Error : "));
  debugSerial.println(inputError);
  debugSerial.print(F("Iterations : "));
  debugSerial.println(iterations);
  debugSerial.println();
}

//=======================================================================//
//activates a task.
//only active tasks are executed regardless of their turn for execution.

void ptScheduler::enable() {
  taskEnabled = true;
}

//=======================================================================//
//suspends a task. interval counter will continue to increment in this mode.

void ptScheduler::suspend() {
  taskSuspended = true;
}

//=======================================================================//
//resume a task from suspended state. the task will resume from where it
//was suspended.

void ptScheduler::resume() {
  taskSuspended = false;
  // intervalCounter = 0;
}

//=======================================================================//
//disables a task from running.
//this will reset all values to their idle state.
//all the user specified values will be preserved.
//this include, mode, skip interval, time, iteration, default interval etc.
//you have to call either enable() or reset() functions to start execution again.

void ptScheduler::disable() {
  taskEnabled = false;
  taskStarted = false;
  cycleStarted = false;
  taskSuspended = false;
  iterationEnded = true;
  taskRunning = false;
  taskRunState = false;

  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  intervalCounter = 0;
  sleepIntervalCounter = 0;
  executionCounter = 0;
  iterationCounter = 0;
  intervalIndex = 0;
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  disable();
  enable();
}

//=======================================================================//
//set the iteration number for a task. the iterations are compared to the
//execution counter each time. when the set number of executions has
//completed, the task will go to sleep mode. the sleep mode is user
//definable.

bool ptScheduler::setIteration (int32_t value) {
  switch (taskMode) {
    case PT_MODE_EPO: //oneshot
    case PT_MODE_EIO:
    case PT_MODE_UPO:
    case PT_MODE_UIO:
      iterations = value;
      iterationsExtended = value;
      return true;
      break;
    
    case PT_MODE_EPS: //oneshot
    case PT_MODE_EIS:
    case PT_MODE_UPS:
    case PT_MODE_UIS:
      //determining how many iteration has executed for a spanning task with odd no. of intervals is not
      //straight forward as oneshot tasks. odd number of intervals produce inverted output after every
      //interval set. for example if the intervals are 1000, 2000 and 3000, the first 1000 ms will be HIGH,
      //next LOW for 2000 ms, then again high for 3000 ms. after that, the pattern is inverted. since 3000 ms
      //was HIGH, next 1000 ms will be LOW and so on. so counting just the HIGH states (execution counter) won't work.
      //we need to calculate how many intervals we have to check to determine if the specified iteration has
      //completed. but the user will specify the number of iterations only.
      iterations = value;
      if ((intervalCount % 2) == 1) {
        iterationsExtended = uint32_t(((intervalCount + 1) / 2) * iterations);
        iterationsExtended--; //we need one less, otherwise there will be one extra active state
      }
      else {
        iterationsExtended = uint32_t((intervalCount * iterations) / 2);
      }
      
      break;
    
    default:
      iterations = 0;
      iterationsExtended = 0;
      inputError = true;
      return false;
      break;
  }
  
  return false;
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
    intervalList[1] = value_2;
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
  if (intervalCount > 0) {
    skipInterval = value;
    skipIntervalSet = true;

    skipTime = 0;
    uint32_t index = 0;

    for (uint32_t i=0; i < value; i++) {
      if (index == intervalCount) {
        index = 0;
      }
      skipTime += intervalList[index];
      index++;
    }
    return true;
  }
  return false;
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
  if (intervalCount > 0) {
    skipIteration = value;
    skipIterationSet = true;

    skipTime = 0;

    for (uint8_t i=0; i < value; i++) {
      for (uint8_t j=0; j < intervalCount; j++) {
        skipTime += intervalList[j];
      }
    }
    return true;
  }
  return false;
}

//=======================================================================//
//skips the specified time before each execution cycle.
//for itertated tasks, this is before each iteration.
//for indefinite tasks, this happens at the beginning once, unless you
//disable or reset it.
//input is time on milliseconds.

bool ptScheduler::setSkipTime (time_ms_t value) {
  if (intervalCount > 0) {
    skipTime = value;
    skipTimeSet = true;
    return true;
  }
  return false;
}

//=======================================================================//
//if any of the user inputs are wrong, the inputError flag is set to true.
//call this function to check if there was any errors.
//calling this function will also reset the flag.

bool ptScheduler::isInputError() {
  if (inputError) {
    inputError = false;
    return true;
  }
  return false;
}

//=======================================================================//
