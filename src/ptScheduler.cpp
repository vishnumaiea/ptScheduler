
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.1
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 11:06:35 AM 29-03-2022, Tuesday

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

ptScheduler::ptScheduler (time_us_t interval_1) {
  intervalSequence = new time_us_t(1);  //create a new list
  // intervalSequence = (time_us_t*) malloc(sizeof(time_us_t));  //create a new list
  intervalSequence[0] = interval_1;
  intervalSequenceLength = 1;
  // intervalSequenceIndex = 0;
  // taskEnabled = true;

  taskMode = PT_MODE_OI;  //onehot mode
  // sleepMode = PT_SLEEP_DISABLE; //task will be disabled when an iteration completes
}

//----------------------------------------------------------------------//
//same as the previous one but you can also specify the mode.

ptScheduler::ptScheduler (uint8_t mode, time_us_t interval_1) {
  intervalSequence = new time_us_t(1);  //create a new list
  intervalSequence[0] = interval_1;
  intervalSequenceLength = 1;
  intervalSequenceIndex = 0;
  taskEnabled = true;
  sleepMode = PT_SLEEP_DISABLE;

  switch (mode) {
    case PT_MODE_OI:
    case PT_MODE_OF:
    case PT_MODE_SI:
    case PT_MODE_SF:
      taskMode = mode;
      break;
    
    default:
      taskMode = PT_MODE_OI;
      inputError = true;
      break;
  }
}

//----------------------------------------------------------------------//
//accepts two intervals. intervals can be same or different.
//works with all mdoes.

// ptScheduler::ptScheduler (uint8_t mode, time_us_t interval_1, time_us_t interval_2) {
//   intervalSequence = new time_us_t(2);  //create a new sequence
//   intervalSequence[0] = interval_1;
//   intervalSequence[1] = interval_2;
//   intervalSequenceLength = 2;
//   intervalSequenceIndex = 0;
//   taskEnabled = true;
//   sleepMode = PT_SLEEP_DISABLE;

//   switch (mode) {
//     case PT_MODE_OI:
//     case PT_MODE_OF:
//     case PT_MODE_SI:
//     case PT_MODE_SF:
//       taskMode = mode;
//       break;
    
//     default:
//       taskMode = PT_MODE_OI;
//       inputError = true;  //when the user has made a mistake
//       break;
//   }
// }

//----------------------------------------------------------------------//
//this accepts a list of intervals. you have to create an array of intervals
//in the globals scope and pass the pointer to this along with the number
//of intervals in the array.

ptScheduler::ptScheduler (uint8_t mode, time_us_t* sequencePtr, uint8_t sequenceLength) {
  if ((sequencePtr != nullptr) && (sequenceLength != 0)) {
    intervalSequence = sequencePtr;
    intervalSequenceLength = sequenceLength;
    
    intervalSequenceIndex = 0;
    taskEnabled = true;
    sleepMode = PT_SLEEP_DISABLE;
    
    switch (mode) {
      case PT_MODE_OI:
      case PT_MODE_OF:
      case PT_MODE_SI:
      case PT_MODE_SF:
        taskMode = mode;
        break;
      
      default:
        taskMode = PT_MODE_OI;
        inputError = true;  //when the user has made a mistake
        break;
    }
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
//you wouldn't believe me if I said it took me a whole day to figure out
//how to implement this logic.

inline void ptScheduler::timeElapsed() {
  uint32_t timeDelta = uint32_t(micros() - entryTime);
  elapsedTime += uint32_t(timeDelta - prevTimeDelta); //this will always return the absolute difference
  prevTimeDelta = timeDelta;
}

//=======================================================================//
//allows you to change modes dynamically.
//returns true if the mode was correct.

bool ptScheduler::setTaskMode (uint8_t mode) {
  switch (mode) {
    case PT_MODE_OI:
    case PT_MODE_OF:
    case PT_MODE_SI:
    case PT_MODE_SF:
      taskMode = mode;
      return true;
      break;
    
    default:
      taskMode = PT_MODE_OI;
      inputError = true;  //when the user has made a mistake
      return false;
      break;
  }
  return false;
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
    case PT_MODE_OI:
    case PT_MODE_OF:
      return oneshot();
      
      break;

    case PT_MODE_SI:
    case PT_MODE_SF:
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
    if ((!taskStarted) && (skipIntervalSet || skipSequenceSet || skipTimeSet)) {
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
      prevTimeDelta = 0;
      entryTime = micros();
      cycleStarted = true;
      repetitionsEnded = false;
      taskStarted = true;
      intervalSequenceIndex = 0;
      intervalCounter = 0;
      // printStats();
    }
    else {  //if an interval cycle has started
      timeElapsed();  //get the elapsed time

      // elapsedTime = micros() - entryTime;

      if (elapsedTime >= intervalSequence[intervalSequenceIndex]) {
        if (intervalSequenceIndex < (intervalSequenceLength-1)) {
          intervalSequenceIndex++;
        }
        else {
          intervalSequenceIndex = 0;
        }

        intervalCounter++;
        exitTime = entryTime + elapsedTime;
        lastElapsedTime = elapsedTime;
        entryTime = micros();

        if (!taskSuspended) {
          if (repetitions > 0) { //if this is a finite repetition task
            if (executionCounter == repetitionsExtended) { //when the specified no. of repetitions have been reached
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
              //the number of repetitions are executed once again.
              executionCounter = 0; //so that we can start a new iteration
              
              // intervalCounter = 0;
              taskRunState = false;
              taskRunning = false;
              repetitionsEnded = true;
              suspendedIntervalCounter = 0;
              // intervalSequenceIndex = 0;  //you have to manually reset this, if you want
              return false;
            }
          }

          //if not an iterated task
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
          suspendedIntervalCounter++; //time elapsed after iteration is complete
          return false;
        }
      }
    }

    //if the task is not suspended, persist the state until the cycle ends.
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

bool ptScheduler::oneshot() {
  if (taskEnabled) {
    //if an execution cycle has not started, yet skip for the time set
    if ((!taskStarted)) {
      if (skipIntervalSet || skipSequenceSet || skipTimeSet) {
        if (entryTime == 0) { //this is one way to find if an exe cycle not started
          elapsedTime = 0;
          entryTime = micros();
          return false;
        }
        else {
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
      // intervalCounter++;  //interval counter increments even if the task is suspended

      //this is the return point that gives you green signal each time 
      if (!taskSuspended) {
        if (repetitions > 0) { //if this is an iterated task
          if (repetetionsCounter == repetitions) { //when the specified no. of repetitions have been reached
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
            //the number of repetitions are executed once again. a side effect of this
            //is that, when you suspend a task before an iteration is completed, this
            //will cause the task to start a new iteration when you resume it next time.
            //you may want the actual iteration to resume. if that's the requirement,
            //save the execution counter value before suspending and reload it before
            //resuming.
            executionCounter = 0;

            //a sequence is not considered ended until all of the intervals have been spanned.
            //oneshot tasks return true at the start of an interval. therefore, execution counter
            //being equal to repetitions does not mean the last interval is elapsed.
            //therefore, the last execution cycle of a oneshot sequence would not return this variable
            //as true. it will be only true after the last interval has elapsed.
            repetitionsEnded = true;

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

    //if cycle has started.
    //check if a time cycle has elapsed.
    else {
      // elapsedTime = millis() - entryTime; //this already takes care of millis() overflow
      timeElapsed();

      if (elapsedTime >= intervalSequence[intervalSequenceIndex]) {
        if (intervalSequenceIndex < (intervalSequenceLength-1)) {
          intervalSequenceIndex++;
        }
        else {
          intervalSequenceIndex = 0;
          //repetitions counter should only increment when the task in not in suspended state.
          if (!taskSuspended) {
            repetetionsCounter++;
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
  debugSerial.println(intervalSequenceLength);
  debugSerial.print(F("Intervals (us) : "));

  for (int i=0; i < intervalSequenceLength; i++) {
    debugSerial.print((int32_t) intervalSequence[i]);

    if (i != (intervalSequenceLength-1)) {
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
  debugSerial.print(F("Repetitions : "));
  debugSerial.println(repetitions);
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
  debugSerial.print(F("Repetitions Counter : "));
  debugSerial.println((uint32_t)repetetionsCounter);
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
  debugSerial.println(repetitionsEnded);
  debugSerial.print(F("Run State : "));
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
  repetitionsEnded = true;
  taskRunning = false;
  taskRunState = false;

  entryTime = 0;
  exitTime = 0;
  elapsedTime = 0;
  intervalCounter = 0;
  suspendedIntervalCounter = 0;
  executionCounter = 0;
  repetetionsCounter = 0;
  intervalSequenceIndex = 0;
}

//=======================================================================//
//a reset reinitializes the task

void ptScheduler::reset() {
  disable();
  enable();
}

//=======================================================================//
//set the iteration number for a task. the repetitions are compared to the
//execution counter each time. when the set number of executions has
//completed, the task will go to sleep mode. the sleep mode is user
//definable.

bool ptScheduler::setRepetition (int32_t value) {
  switch (taskMode) {
    case PT_MODE_OI:
    case PT_MODE_OF:
      repetitions = value;
      repetitionsExtended = value;
      return true;
      break;
    
    case PT_MODE_SI:
    case PT_MODE_SF:
      //determining how many iteration has executed for a spanning task with odd no. of intervals is not
      //straight forward as oneshot tasks. odd number of intervals produce inverted output after every
      //interval set. for example if the intervals are 1000, 2000 and 3000, the first 1000 ms will be HIGH,
      //next LOW for 2000 ms, then again high for 3000 ms. after that, the pattern is inverted. since 3000 ms
      //was HIGH, next 1000 ms will be LOW and so on. so counting just the HIGH states (execution counter) won't work.
      //we need to calculate how many intervals we have to check to determine if the specified iteration has
      //completed. but the user will specify the number of repetitions only.
      repetitions = value;
      if ((intervalSequenceLength % 2) == 1) {
        repetitionsExtended = uint32_t(((intervalSequenceLength + 1) / 2) * repetitions);
        repetitionsExtended--; //we need one less, otherwise there will be one extra active state
      }
      else {
        repetitionsExtended = uint32_t((intervalSequenceLength * repetitions) / 2);
      }
      
      break;
    
    default:
      repetitions = 0;
      repetitionsExtended = 0;
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

bool ptScheduler::setInterval (time_us_t value) {
  if (intervalSequenceLength > 0) {
    intervalSequence[0] = value;
    return true;
  }
  else {
    inputError = true;
    return false;
  }
}

//----------------------------------------------------------------------//

bool ptScheduler::setInterval (time_us_t value_1, time_us_t value_2) {
  if (intervalSequenceLength > 1) {
    intervalSequence[0] = value_1;
    intervalSequence[1] = value_2;
    return true;
  }
  else {
    inputError = true;
    return false;
  }
}

//=======================================================================//
//skips the specified many intervals at the beginning.
//this is different from skipping repetitions.
//if you have unequal intervals set, each of them are picked in order,
//until it is equal to the intervals to set.
//skip time is calculated from this selected many intervals by adding them.
//if you set both skip interval and skip iteration, the last one determines
//skip time. input is a positive integer.

bool ptScheduler::setSkipInterval (uint32_t value) {
  if (intervalSequenceLength > 0) {
    skipInterval = value;
    skipIntervalSet = true;

    skipTime = 0;
    uint32_t index = 0;

    for (uint32_t i=0; i < value; i++) {
      if (index == intervalSequenceLength) {
        index = 0;
      }
      skipTime += intervalSequence[index];
      index++;
    }
    return true;
  }
  return false;
}

//=======================================================================//
//skips the specified many repetitions at the beginning.
//an iteration is the set of all the specified intervals.
//eg. if you have specified 3 unequal intervals, they together form an iteration.
//for periodic oneshot (mode 1) tasks, and tasks with equal intervals
//this is same as setting skip intervals.
//when you set a skip iteration, the skip time is automatically calculated.
//if you have unequal intervals,each set is summed and multiplied by the
//itertations to skip, to find the time.
//input is a positive integer.

bool ptScheduler::setSkipSequence (uint32_t value) {
  if (intervalSequenceLength > 0) {
    skipSequence = value;
    skipSequenceSet = true;

    skipTime = 0;

    for (uint8_t i=0; i < value; i++) {
      for (uint8_t j=0; j < intervalSequenceLength; j++) {
        skipTime += intervalSequence[j];
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

bool ptScheduler::setSkipTime (time_us_t value) {
  if (intervalSequenceLength > 0) {
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
