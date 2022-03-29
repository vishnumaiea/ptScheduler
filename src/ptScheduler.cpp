
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.1.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 23:51:40 PM 29-03-2022, Tuesday

//=======================================================================//

//  Read the header file for a better understanding of how this library is
//  organized. I have added plenty of comments to help you understand it.

//=======================================================================//
//includes

#include "ptScheduler.h"

//=======================================================================//
//constructors

//creates the basic type of task.
//accepts a single interval in microseconds.
//the list will be only one interval long.
//adding additional intervals later will fail.
//the default mode is ONESHOT.

ptScheduler::ptScheduler (time_us_t interval_1) {
  sequenceList = new time_us_t(1);  //create a new interval list
  sequenceList[0] = interval_1;
  sequenceLength = 1;
  // sequenceIndex = 0;
  // taskEnabled = true;

  taskMode = PT_MODE_ONESHOT;
  // sleepMode = PT_SLEEP_DISABLE;
}

//----------------------------------------------------------------------//
//same as the previous one but you can also specify the mode.
//fallback mode is ONESHOT, in case of input error.
//default sleep mode is DISABLE.

ptScheduler::ptScheduler (uint8_t mode, time_us_t interval_1) {
  sequenceList = new time_us_t(1);  //create a new list
  sequenceList[0] = interval_1;
  sequenceLength = 1;
  sequenceIndex = 0;
  taskEnabled = true;
  sleepMode = PT_SLEEP_DISABLE;

  switch (mode) {
    case PT_MODE_ONESHOT:
    case PT_MODE_SPANNING:
      taskMode = mode;
      break;
    
    default:
      taskMode = PT_MODE_ONESHOT;
      inputError = true;
      break;
  }
}

//----------------------------------------------------------------------//
//this accepts a list of intervals. you have to create an array of intervals
//in the global scope and pass the pointer to this along with the number
//of intervals in the array. the list of intervals is called an interval
//sequence and the number of intervals as sequence length. 

ptScheduler::ptScheduler (uint8_t mode, time_us_t* sequencePtr, uint8_t sequenceLen) {
  if ((sequencePtr != nullptr) && (sequenceLen != 0)) {
    sequenceList = sequencePtr;
    sequenceLength = sequenceLen;
    
    sequenceIndex = 0;
    taskEnabled = true;
    sleepMode = PT_SLEEP_DISABLE;
    
    switch (mode) {
      case PT_MODE_ONESHOT:
      case PT_MODE_SPANNING:
        taskMode = mode;
        break;
      
      default:
        taskMode = PT_MODE_ONESHOT;
        inputError = true;  //when the user has made a mistake
        break;
    }
  }
  else {  //if the input parameters are invalid
    ptScheduler(PT_TIME_DEFAULT);
    inputError = true;
  }
}

//=======================================================================//
//destructor

ptScheduler::~ptScheduler() {
  
}

//=======================================================================//
//you wouldn't believe me if I said it took me a whole day to figure out
//how to implement this logic.
//returns the time elapsed from entry time, in microseconds.
//this logic is designed to deal with overflow events.

// void ptScheduler::getTimeElapsed() {
//   uint32_t timeDelta = uint32_t(GET_MICROS() - entryTime);
//   elapsedTime += uint32_t(timeDelta - prevTimeDelta); //this will always return the absolute difference
//   prevTimeDelta = timeDelta;
// }

void ptScheduler::getTimeElapsed() {
  microsValue = GET_MICROS();
  timeDelta = uint32_t(microsValue - entryTime);
  uint32_t diff = uint32_t(timeDelta - prevTimeDelta);

  elapsedTime += diff; 
  prevTimeDelta = timeDelta;
}

//=======================================================================//
//allows you to change modes dynamically.
//returns true if the mode is valid.

bool ptScheduler::setTaskMode (uint8_t mode) {
  switch (mode) {
    case PT_MODE_ONESHOT:
    case PT_MODE_SPANNING:
      taskMode = mode;
      return true;
      break;
    
    default:
      taskMode = PT_MODE_ONESHOT;
      inputError = true;  //when the user has made a mistake
      return false;
      break;
  }
  return false;
}

//=======================================================================//
//sleep is the event when a task finishes a number of finite sequenceRepetition.
//a task can end the sequenceRepetition in two ways, either SUSPEND or DISABLE.
//DISBLE completely turns off the task and no counters or variables will
//change during DISABLE. you can only restart a task by enabling it.
//SUSPEND on the other hand temporarily suspends the task activity
//by immediately returning false always, but keep some of the counters running.
//suspened takss can be resumed based on how long they have been
//suspended. this is not possible with disabled tasks.
//the suspendedIntervalCounter is the counter that will be running
//during suspension.

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
//do not use other blocking delays inside or outside those blocks.

bool ptScheduler::call() {
  switch (taskMode) {
    case PT_MODE_ONESHOT:
      return oneshot();
      
      break;

    case PT_MODE_SPANNING:
      return spanning();

      break;

    default:
      break;
  }
  
  return false;
}

//=======================================================================//
//implements the SPANNING task logic.
//the return state of a spanning task will persist until the ongoing interval
//is ended. if you set the state to true and wait for 2 seconds, then the sate
//will remain true until 2 seconds are passed. this is unlike ONESHOT logic
//which return true only moemetarily.

bool ptScheduler::spanning() {
  if (taskEnabled) {
    //if an execution cycle has not started yet, skip for the time set.
    if ((!taskStarted) && (skipIntervalSet || skipSequenceSet || skipTimeSet)) {
      if (entryTime == 0) { //this is one way to find if an exe cycle has not started
        elapsedTime = 0;
        entryTime = GET_MICROS();
        return false;
      }
      else {
        elapsedTime = GET_MICROS() - entryTime;

        if (elapsedTime < skipTime) { //skipTime is set when skip time or skip interval is set
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
      prevTimeDelta = 0;
      entryTime = GET_MICROS();
      cycleStarted = true;
      sequenceRepetitionEnded = false;
      taskStarted = true;
      sequenceIndex = 0;
      intervalCounter = 0;
      // printStats();
    }
    else {  //if an interval cycle has started
      getTimeElapsed();  //get the elapsed time since entry time

      if (elapsedTime >= sequenceList[sequenceIndex]) {
        if (sequenceIndex < (sequenceLength-1)) {
          sequenceIndex++;
        }
        else {
          sequenceIndex = 0;
        }

        intervalCounter++;
        exitTime = entryTime + elapsedTime;
        lastElapsedTime = elapsedTime;
        elapsedTime = 0;
        entryTime = GET_MICROS(); //we are entering a new cycle
        prevTimeDelta = 0;  //so we can reset these
        timeDelta = 0;

        if (!taskSuspended) {
          if (sequenceRepetition > 0) { //if this is a finite repetition task
            if (executionCounter == sequenceRepetitionExtended) { //when the specified no. of sequenceRepetition have been reached
              if (sleepMode == PT_SLEEP_DISABLE) {
                disable(); //interval counter will not run in this mode
              }
              else {
                //this is the self-suspend mode.
                suspend();
              }

              //when a task suspends itself, it also resets the execution counter.
              //because of this, when you resume a suspended task the next time,
              //the number of sequenceRepetition are executed once again.
              executionCounter = 0;
              
              // intervalCounter = 0;
              taskRunState = false;
              taskRunning = false;
              sequenceRepetitionEnded = true;
              suspendedIntervalCounter = 0;
              // sequenceIndex = 0;  //you have to manually reset this, if you want
              return false;
            }
          }

          taskRunState = (taskRunState) ? false : true; //toggle the state

          if (taskRunState) {
            executionCounter++;
            taskRunning = true;
            // printStats();
          }
          else {
            taskRunning = false;
          }
        }
        else {  //if suspended
          // printStats();
          suspendedIntervalCounter++; //counts how long a task remains suspended

          //without this, the states may be swapped after resuming a task.
          taskRunState = (taskRunState) ? false : true; //toggle the state
          
          //if you return taskRunState instead, you can generate pulses on rising and falling edges
          return false;
        }
      }
    }

    //if the task is not suspended, persist the state until the interval ends.
    if (!taskSuspended) {
      return taskRunState;
    }

    //if the task is in suspended mode, always return false.
    return false;
  }

  else { //if not activated
    return false;
  }
}

//=======================================================================//
//implements the logic of oneshot tasks.
//ONESHOT returns true momentarily during the rising edge of an interval.

bool ptScheduler::oneshot() {
  if (taskEnabled) {
    //if an execution cycle has not started yet, skip for the time set.
    if ((!taskStarted)) {
      if (skipIntervalSet || skipSequenceSet || skipTimeSet) {
        if (entryTime == 0) { //this is one way to find if an exe cycle not started
          elapsedTime = 0;
          entryTime = GET_MICROS();
          return false;
        }
        else {
          if ((GET_MICROS() - entryTime) < skipTime) {
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
      timeDelta = 0;
      entryTime = GET_MICROS();
      cycleStarted = true;
      // intervalCounter++;

      //this is the return point that gives you green signal each time.
      if (!taskSuspended) {
        if (sequenceRepetition > 0) { //if this is an iterated task
          if (sequenceRepetitionCounter == sequenceRepetition) { //when the specified no. of sequenceRepetition have been reached
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
            //the number of sequenceRepetition are executed once again. a side effect of this
            //is that, when you suspend a task before an iteration is completed, this
            //will cause the task to start a new iteration when you resume it next time.
            //you may want the actual iteration to resume. if that's the requirement,
            //save the execution counter value before suspending and reload it before
            //resuming.
            executionCounter = 0;

            //a sequence is not considered ended until all of the intervals have been spanned.
            //oneshot tasks return true at the start of an interval. therefore, execution counter
            //being equal to sequenceRepetition does not mean the last interval is elapsed.
            //therefore, the last execution cycle of a oneshot sequence would not return this variable
            //as true. it will be only true after the last interval in the sequence has elapsed.
            sequenceRepetitionEnded = true;

            suspendedIntervalCounter = 0;
            return false;
          }
        }
        executionCounter++;
        return true;
      }
      else {
        suspendedIntervalCounter++;
        return false;
      }
    }

    //check if a time cycle has elapsed.
    else {
      getTimeElapsed();

      if (elapsedTime >= sequenceList[sequenceIndex]) {
        if (sequenceIndex < (sequenceLength-1)) {
          sequenceIndex++;
        }
        else {
          sequenceIndex = 0;
          //sequenceRepetition counter should only increment when the task in not in suspended state.
          if (!taskSuspended) {
            sequenceRepetitionCounter++;
          }
        }

        cycleStarted = false; //so that we can start a new time cycle
        exitTime = entryTime + elapsedTime;
        lastElapsedTime = elapsedTime;
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
//unfortunately, print() does not accept 64-bit values.
//so values like timespans will be tructated to 32-bit.

void ptScheduler::printStats() {
  debugSerial.print(F("Interval Sequence Length : "));
  debugSerial.println(sequenceLength);
  debugSerial.print(F("Intervals (us) : "));

  for (int i=0; i < sequenceLength; i++) {
    debugSerial.print((int32_t) sequenceList[i]);

    if (i != (sequenceLength-1)) {
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
  debugSerial.print(F("Skip Sequence : "));
  debugSerial.println(skipSequence);
  debugSerial.print(F("Skip Time : "));
  debugSerial.println((int32_t) skipTime);
  debugSerial.print(F("sequenceRepetition : "));
  debugSerial.println(sequenceRepetition);
  debugSerial.print(F("Entry Time : "));
  debugSerial.println((int32_t) entryTime);
  debugSerial.print(F("Elapsed Time : "));
  debugSerial.println((int32_t) elapsedTime);
  debugSerial.print(F("Last Elapsed Time : "));
  debugSerial.println((int32_t) lastElapsedTime);
  debugSerial.print(F("Exit Time : "));
  debugSerial.println((int32_t) exitTime);
  debugSerial.print(F("Interval Counter : "));
  debugSerial.println((uint32_t)intervalCounter);
  debugSerial.print(F("Sleep Interval Counter : "));
  debugSerial.println((uint32_t)suspendedIntervalCounter);
  debugSerial.print(F("Execution Counter : "));
  debugSerial.println((uint32_t)executionCounter);
  debugSerial.print(F("sequenceRepetition Counter : "));
  debugSerial.println((uint32_t)sequenceRepetitionCounter);
  debugSerial.print(F("Task Enabled ? : "));
  debugSerial.println(taskEnabled);
  debugSerial.print(F("Task Suspended ? : "));
  debugSerial.println(taskSuspended);
  debugSerial.print(F("Task Started ? : "));
  debugSerial.println(taskStarted);
  debugSerial.print(F("Cycle Started ? : "));
  debugSerial.println(cycleStarted);
  debugSerial.print(F("Task Running ? : "));
  debugSerial.println(taskRunning);
  debugSerial.print(F("Task Ended ? : "));
  debugSerial.println(sequenceRepetitionEnded);
  debugSerial.print(F("Task Run State : "));
  debugSerial.println(taskRunState);
  debugSerial.print(F("Input Error : "));
  debugSerial.println(inputError);
  debugSerial.println();
}

//=======================================================================//
//activates a task.
//only active tasks are executed regardless of their turn for execution.

void ptScheduler::enable() {
  taskEnabled = true;
}

//=======================================================================//
//suspends a task. suspendedIntervalCounter will continue to increment in this mode.

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
//all user specified values will be preserved.
//this include, mode, skip interval, time, sequenceRepetition, default interval etc.
//you have to call either enable() or reset() functions to start execution.

void ptScheduler::disable() {
  taskEnabled = false;
  taskStarted = false;
  cycleStarted = false;
  taskSuspended = false;
  sequenceRepetitionEnded = true;
  taskRunning = false;
  taskRunState = false;

  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  timeDelta = 0;
  prevTimeDelta = 0;
  intervalCounter = 0;
  suspendedIntervalCounter = 0;
  executionCounter = 0;
  sequenceRepetitionCounter = 0;
  sequenceIndex = 0;
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  disable();
  enable();
}

//=======================================================================//
//sets how many times the interval sequence has to be executed.
//execution of a sequence each time is called a repetition.
//this only applies to finite tasks.

bool ptScheduler::setSequenceRepetition (int32_t value) {
  switch (taskMode) {
    case PT_MODE_ONESHOT:
      sequenceRepetition = value;
      sequenceRepetitionExtended = value;
      return true;
      break;
    
    case PT_MODE_SPANNING:
      //determining how many sequenceRepetition have executed for a spanning task with odd no. of intervals is not
      //straight forward as oneshot tasks. odd number of intervals produce inverted output after every
      //interval set. for example if the intervals are 1000, 2000 and 3000, the first 1000 ms will be HIGH,
      //next LOW for 2000 ms, then again high for 3000 ms. after that, the pattern is inverted. since 3000 ms
      //was HIGH, next 1000 ms will be LOW and so on. so counting just the HIGH states (execution counter) won't work.
      //we need to calculate how many intervals we have to check to determine if the specified repetition has
      //completed. but the user will specify the number of sequenceRepetition only.
      sequenceRepetition = value;
      
      if ((sequenceLength % 2) == 1) {
        sequenceRepetitionExtended = uint32_t(((sequenceLength + 1) / 2) * sequenceRepetition);
        sequenceRepetitionExtended--; //we need one less, otherwise there will be one extra active state
      }
      else {
        sequenceRepetitionExtended = uint32_t((sequenceLength * sequenceRepetition) / 2);
      }
      
      break;
    
    default:
      sequenceRepetition = 0;
      sequenceRepetitionExtended = 0;
      inputError = true;
      return false;
      break;
  }
  return false;
}

//=======================================================================//
//set the first interval value of an interval sequence.
//input is time in microseconds.

bool ptScheduler::setInterval (time_us_t value) {
  if (sequenceLength > 0) {
    sequenceList[0] = value;
    return true;
  }
  else {
    inputError = true;
    return false;
  }
}

//=======================================================================//
//skips the specified many intervals at the beginning.
//this is different from skipping a whole sequence.
//if you have unequal intervals set, each of them are picked in order,
//until it is equal to the intervals to skip.
//skip time is calculated from this selected many intervals by summing them.
//if you set both skip interval and skip iteration, the last one determines
//skip time. input is a positive integer.

bool ptScheduler::setSkipInterval (uint32_t value) {
  if (sequenceLength > 0) {
    skipInterval = value;

    if (value == 0) {
      skipIntervalSet = false;
      skipTime = 0;
      return true;
    }

    skipIntervalSet = true;
    skipTime = 0;
    uint32_t index = 0;

    for (uint32_t i=0; i < value; i++) {
      if (index == sequenceLength) {
        index = 0;
      }
      skipTime += sequenceList[index];
      index++;
    }
    return true;
  }
  return false;
}

//=======================================================================//
//skips the specified many sequences at the beginning.
//a sequence is the set of all the specified intervals.

bool ptScheduler::setSkipSequence (uint32_t value) {
  if (sequenceLength > 0) {
    skipSequence = value;

    if (value == 0) {
      skipSequenceSet = false;
      skipTime = 0;
      return true;
    }
    
    skipSequenceSet = true;
    skipTime = 0;

    for (uint8_t i=0; i < value; i++) {
      for (uint8_t j=0; j < sequenceLength; j++) {
        skipTime += sequenceList[j];
      }
    }
    return true;
  }
  return false;
}

//=======================================================================//
//skip the specified time before executing the task.

bool ptScheduler::setSkipTime (time_us_t value) {
  if (sequenceLength > 0) {
    skipTime = value;
    
    if (value == 0) {
      skipTimeSet = false;
    }

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
