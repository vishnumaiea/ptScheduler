
//==============================================================================//
/**
 * @file ptScheduler.cpp
 * @author Vishnu Mohanan (@vishnumaiea)
 * @brief "Pretty tiny Scheduler" is an Arduino library for writing non-blocking
 * periodic tasks without using delay() or millis() routines.
 * 
 * Read the header file for a better understanding of how this library is
 * organized. I have added plenty of comments to help you with it.
 * 
 * @version 2.2.0
 * @link https://github.com/vishnumaiea/ptScheduler
 * @date Last modified : +05:30 09:53:39 AM 27-06-2023, Tuesday
 * @copyright License: MIT
 * 
 */
//==============================================================================//
// Includes

#include "ptScheduler.h"

//==============================================================================//
// Constructors

/**
 * @brief Creates the basic type of task. Accepts a single interval in microseconds.
 * The list will be only one interval long. Adding additional intervals later
 * will fail. The default mode is ONESHOT.
 * 
 * @param interval_1 Interval value in microseconds.
 * @return ptScheduler:: 
 */
ptScheduler:: ptScheduler (time_us_t interval_1) {
  sequenceList = new time_us_t (1);  //create a new interval list
  sequenceList [0] = interval_1;
  sequenceLength = 1;
  // sequenceIndex = 0;
  // taskEnabled = true;

  taskMode = PT_MODE_ONESHOT;
  // sleepMode = PT_SLEEP_DISABLE;
}

//----------------------------------------------------------------------------//
/**
 * @brief Creates the basic type of task. Accepts a single interval in microseconds
 * and the working mode. Fallback mode is ONESHOT, in case of input error.
 * Default sleep mode is DISABLE; the task will be disabled after executing
 * the first sequence. inputError is set to true in case of input error.
 * 
 * @param mode The working mode; can be PT_MODE_ONESHOT or PT_MODE_SPANNING.
 * @param interval_1 Interval value in microseconds.
 * @return ptScheduler:: 
 */
ptScheduler:: ptScheduler (uint8_t mode, time_us_t interval_1) {
  sequenceList = new time_us_t (1);  // create a new list
  sequenceList [0] = interval_1;
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

//----------------------------------------------------------------------------//
/**
 * @brief This accepts a list of intervals. You have to create an array of
 * intervals in the global scope and pass the pointer to this along with the
 * number of intervals in the array. The list of intervals is called an interval
 * sequence and the number of intervals as sequence length. The default mode is
 * ONESHOT. The default sleep mode is DISABLE; the task will be disabled after
 * executing the first sequence. inputError is set to true in case of input
 * error.
 * 
 * @param mode The working mode; can be PT_MODE_ONESHOT or PT_MODE_SPANNING.
 * @param sequencePtr Pointer to the interval sequence (an array).
 * @param sequenceLen Number of intervals in the sequence.
 * @return ptScheduler:: 
 */
ptScheduler:: ptScheduler (uint8_t mode, time_us_t* sequencePtr, uint8_t sequenceLen) {
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
        inputError = true;  // when the user has made a mistake
        break;
    }
  }
  else {  // if the input parameters are invalid
    ptScheduler (PT_TIME_DEFAULT);
    inputError = true;
  }
}

//==============================================================================//
/**
 * @brief Does nothing for now.
 * 
 * @return ptScheduler:: 
 */
ptScheduler:: ~ptScheduler() {
  
}

//==============================================================================//
/**
 * @brief Calculates the time elapsed from the entry time, in microseconds.
 * The value is stored in the elapsedTime variable.
 * timeDelta is the difference between the current time and the entry time.
 * But in overflow events it may not indicate the actual difference in time.
 * Therefore, the actual real difference is calculated by subtracting the
 * previous timeDelta from the current timeDelta. This is the actual time
 * elapsed since the last time this function was called.
 * 
 */
void ptScheduler:: getTimeElapsed() {
  microsValue = GET_MICROS();
  timeDelta = uint32_t (microsValue - entryTime);
  uint32_t diff = uint32_t (timeDelta - prevTimeDelta);

  elapsedTime += diff; 
  prevTimeDelta = timeDelta;
}

//==============================================================================//
/**
 * @brief Allows you to change modes dynamically. Returns true if the mode is
 * valid. inputError is set to true in case of input error. Fallback mode is
 * ONESHOT.
 * 
 * @param mode The task working mode. Can be PT_MODE_ONESHOT or PT_MODE_SPANNING.
 * @return true Mode is valid.
 * @return false Mode is invalid.
 */
bool ptScheduler:: setTaskMode (uint8_t mode) {
  switch (mode) {
    case PT_MODE_ONESHOT:
    case PT_MODE_SPANNING:
      taskMode = mode;
      return true;
      break;
    
    default:
      taskMode = PT_MODE_ONESHOT;
      inputError = true;  // when the user has made a mistake
      return false;
      break;
  }
  return false;
}

//==============================================================================//
/**
 * @brief Sleep is the event when a task finishes a number of finite
 * sequence repetitions. A task can end the sequence repetition in two ways,
 * either SUSPEND or DISABLE the task after completion. DISBLE completely
 * turns off the task and no counsters/variables will increment/decrement.
 * You can only restart a task by enabling it. SUSPEND on the other hand
 * temporarily suspends the task activity by immediately returning false always,
 * but keeps some of the counters running. Suspened tasks can be resumed based
 * on how long they have been suspended. This is not possible with disabled tasks.
 * The suspendedIntervalCounter is the counter that will be incrementing during
 * suspension.
 * 
 * Returns true if the mode is valid. inputError is set to true in
 * case of input error. Fallback mode is DISABLE.
 * 
 * @param mode The task sleep mode. Can be PT_SLEEP_DISABLE or PT_SLEEP_SUSPEND.
 * @return true Mode is valid.
 * @return false Mode is invalid.
 */
bool ptScheduler:: setSleepMode (uint8_t mode) {
  switch (mode) { // check if the input mode is valid
    case PT_SLEEP_DISABLE:  // Disable mode
    case PT_SLEEP_SUSPEND:  // Suspend mode
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

//==============================================================================//
/**
 * @brief Determines whether a task should run or not. Enclose this function
 * call in any conditional clause to run the code block. Do not use any other
 * blocking delays inside or outside those blocks, which will defeat the 
 * purpose of this library.
 * 
 * For Oneshot tasks, the return state will be true only momentarily, after
 * an interval is elapsed. This allows you to run code blocks only once in
 * a given interval. For Spanning tasks, the return state will persist until
 * the ongoing interval is ended.
 * 
 * @return true Task to be executed.
 * @return false Task not to be executed.
 */
bool ptScheduler:: call() {
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

//==============================================================================//
/**
 * @brief Implements the SPANNING task logic. The return state of a spanning
 * task will persist until the ongoing interval is ended. If you set the state
 * to true and wait for 2 seconds, then the sate will remain true until 2
 * seconds are passed. This is unlike ONESHOT logic which returns true only
 * momentarily.
 * 
 * @return true Task to be executed.
 * @return false Task not to be executed.
 */
bool ptScheduler:: spanning() {
  if (taskEnabled) {
    // If an execution cycle has not started yet, defer the task until the time set by the
    // user (called skip time). The user can specify the skip time in terms of time, sequence
    // or interval.
    if ((!taskStarted) && (skipIntervalSet || skipSequenceSet || skipTimeSet)) {
      if (entryTime == 0) { // this is one way to find if an execution cycle has not started
        elapsedTime = 0;
        entryTime = GET_MICROS();
        return false;
      }
      else {
        elapsedTime = GET_MICROS() - entryTime;

        if (elapsedTime < skipTime) { // skipTime is set when skip time or skip interval is set
          return false;
        }
        else {
          taskStarted = true;
          taskRunState = true;
        }
      }
    }

    // If an interval cycle has not started yet, then initialize all variables with
    // a known default state. We also save the entry time at this time.
    // For spanning tasks, this only happens once.
    if (!cycleStarted) {
      taskRunState = true;
      taskRunning = true;
      executionCounter++; // This increments before the interval counter
      elapsedTime = 0;  // Reset so that we can start a new cycle
      prevTimeDelta = 0;
      entryTime = GET_MICROS(); // Get the entry time
      cycleStarted = true;
      sequenceRepetitionEnded = false;
      taskStarted = true;
      sequenceIndex = 0;
      intervalCounter = 0;
      // printStats();
    }
    else {  // If an interval cycle has started
      getTimeElapsed();  // Get the elapsed time since entry time

      if (elapsedTime >= sequenceList [sequenceIndex]) { // Check if the elapsed time is greater than an interval in the sequence list.
        if (sequenceIndex < (sequenceLength - 1)) { // Move to the next interval in the sequence list.
          sequenceIndex++;
        }
        else { // Else reset to the first interval in the sequence list.
          sequenceIndex = 0;
        }

        intervalCounter++; // Counter increments after an interval (not sequence) is completed.
        exitTime = entryTime + elapsedTime; // Save the exit time
        lastElapsedTime = elapsedTime;
        entryTime = GET_MICROS(); // After an interval is completed, get the new entry time.
        elapsedTime = 0;
        prevTimeDelta = 0;
        timeDelta = 0;

        if (taskSuspended) { // If the task gets suspended.
          // printStats();
          suspendedIntervalCounter++; // Counts how long a task remains suspended.
          
          // The following line restores a suspended task to the same state during which it was suspended.
          // Without this, the states may be swapped after resuming a task.
          taskRunState = (taskRunState) ? false : true; // Toggle the state
          
          // If you return taskRunState instead, you can generate pulses on rising and falling edges.
          // Try it out and see what happens.
          return false;
        }

        if (!taskSuspended) { // If the task is still active.
          if (sequenceRepetition > 0) { // If this is a finite repetition task.
            if (executionCounter >= sequenceRepetitionExtended) { // Check if the task has completed all its repetitions.
              // If yes, put the task to sleep; either DISABLE or SUSPEND.
              if (sleepMode == PT_SLEEP_DISABLE) {
                // No counters will be running in disabled mode
                disable();
              }
              else {
                // But in suspend mode, some counters will be running
                // which allows you to resume the task based on how long it has been suspended.
                suspend();
              }
              
              // intervalCounter = 0;
              taskRunState = false;
              taskRunning = false;
              sequenceRepetitionEnded = true;
              suspendedIntervalCounter = 0;
              // sequenceIndex = 0;  // You have to manually reset this, if you want
              return false;
            }
          }

          // Determine the next state of the task. Since we are here because an interval has ended,
          // we can toggle the task state.
          taskRunState = (taskRunState) ? false : true;

          // If the task state is true, then we can also increment the execution counter.
          // The execution counter keeps track of how many times the task has been executed (returned true).
          if (taskRunState) {
            executionCounter++;
            taskRunning = true;
            // printStats();
          }
          else {
            taskRunning = false;
          }
        }
      }
    }

    // If the task is not suspended, persist the state until the interval ends.
    if (!taskSuspended) {
      return taskRunState;
    }

    // If the task is in suspended mode, always return false.
    return false;
  }

  else { // If the task is not enabled.
    return false;
  }
}

//==============================================================================//
/**
 * @brief Implements the logic of ONESHOT tasks. ONESHOT returns true momentarily during the
 * rising edge of an interval.
 * 
 * @return true Task to be executed.
 * @return false Task not to be executed.
 */
bool ptScheduler:: oneshot() {
  if (taskEnabled) {
    // If an execution cycle has not started yet, defer the task until the time set by the
    // user (called skip time). The user can specify the skip time in terms of time, sequence
    // or interval.
    if ((!taskStarted)) {
      if (skipIntervalSet || skipSequenceSet || skipTimeSet) {
        if (entryTime == 0) { // This is one way to find if an execution cycle has not started.
          elapsedTime = 0;
          entryTime = GET_MICROS(); // Get the entry time.
          return false;
        }
        else { // If we have already entered the skip duration.
          if ((GET_MICROS() - entryTime) < skipTime) {
            return false;
          }
          else {
            taskStarted = true; // Start the task after the skip duration is elapsed.
          }
        }
      }
    }

    // Start a new interval cycle, if it has not already.
    // We get a true signal every time an interval starts.
    // If you don't want a oneshot task to be executed at the start of a new interval cycle,
    // then simply set a skip time.
    if (!cycleStarted) {
      elapsedTime = 0;
      prevTimeDelta = 0;
      timeDelta = 0;
      entryTime = GET_MICROS(); // Get the entry time.
      cycleStarted = true;
      // intervalCounter++;

      // If the tasks gets suspended, always return false.
      // But also increment the suspended interval counter so that we can use it to
      // resume the task.
      if (taskSuspended) {
        suspendedIntervalCounter++;
        return false;
      }

      // This is the return point that gives you green signal each time.
      if (!taskSuspended) {
        // Check if this is a finite repetition task.
        // If yes, we must either disable or suspend the task after the specified number of repetitions.
        if (sequenceRepetition > 0) {
          if (sequenceRepetitionCounter >= sequenceRepetition) {
            if (sleepMode == PT_SLEEP_DISABLE) {
              disable(); // Interval counter will not run in this mode.
            }
            else {
              // This is the self-suspend mode.
              // Interval counter runs and you can resume the task when you need.
              suspend();
            }

            // A sequence is not considered ended until all of the intervals have been elapsed.
            // Oneshot tasks return true at the start of an interval. Therefore, execution counter
            // being equal to sequenceRepetition does not mean the last interval is elapsed.
            // Therefore, the last execution cycle of a oneshot sequence would not return this variable
            // as true. It will be only true after the last interval in the sequence has elapsed.
            sequenceRepetitionEnded = true;

            // The suspended interval counter is reset at this point so that any subsequent
            // invocation of the task will increment it from 0.
            suspendedIntervalCounter = 0;

            // We return false because the task is now either disabled or suspended.
            return false;
          }
        }
        executionCounter++;
        return true;
      }
    }

    // If an interval cycle has already started, then we need to check if the interval has ended or not.
    if (cycleStarted) {
      getTimeElapsed();

      // If the current interval in the sequence list is not elapsed.
      if (elapsedTime < sequenceList [sequenceIndex]) {
        return false;
      }

      // If the current interval in the sequence is elapsed.
      if (sequenceIndex < (sequenceLength - 1)) { // Check if we have reached the end of the list.
        sequenceIndex++; // If not, move to the next interval.
      }
      else { // If all intervals in the sequence have been elapsed.
        sequenceIndex = 0;
        // sequenceRepetition counter should only increment when the task in not in suspended state.
        if (!taskSuspended) {
          sequenceRepetitionCounter++;
        }
      }

      cycleStarted = false; // Reset so that we can start a new interval cycle.
      exitTime = entryTime + elapsedTime; // Save the exit time.
      lastElapsedTime = elapsedTime;
      return false;
    }
  }

  else {
    return false; //if not activated
  }

  return false;
}

//==============================================================================//
/**
 * @brief Prints the state variables and counters of the task.
 * Unfortunately, print() does not accept 64-bit values.
 * So values like timespans will be truncated to 32-bit.
 * 
 */
void ptScheduler:: printStats() {
  debugSerial.print (F("Interval Sequence Length: "));
  debugSerial.println (sequenceLength);
  debugSerial.print (F("Intervals (us): "));

  for (int i = 0; i < sequenceLength; i++) {
    debugSerial.print ((int32_t) sequenceList [i]);

    if (i != (sequenceLength - 1)) {
      debugSerial.print (F(", "));
    }
    else {
      debugSerial.println();
    }
  }
    
  debugSerial.print (F ("Task Mode: "));
  debugSerial.println (taskMode);
  debugSerial.print (F ("Sleep Mode: "));
  debugSerial.println (sleepMode);
  debugSerial.print (F ("Skip Interval: "));
  debugSerial.println (skipInterval);
  debugSerial.print (F ("Skip Sequence: "));
  debugSerial.println (skipSequence);
  debugSerial.print (F ("Skip Time: "));
  debugSerial.println ((int32_t) skipTime);
  debugSerial.print (F ("sequenceRepetition: "));
  debugSerial.println (sequenceRepetition);
  debugSerial.print (F ("Entry Time: "));
  debugSerial.println ((int32_t) entryTime);
  debugSerial.print (F ("Elapsed Time: "));
  debugSerial.println ((int32_t) elapsedTime);
  debugSerial.print (F ("Last Elapsed Time: "));
  debugSerial.println ((int32_t) lastElapsedTime);
  debugSerial.print (F ("Exit Time: "));
  debugSerial.println ((int32_t) exitTime);
  debugSerial.print (F ("Interval Counter: "));
  debugSerial.println ((uint32_t) intervalCounter);
  debugSerial.print (F ("Sleep Interval Counter: "));
  debugSerial.println ((uint32_t) suspendedIntervalCounter);
  debugSerial.print (F ("Execution Counter: "));
  debugSerial.println ((uint32_t) executionCounter);
  debugSerial.print (F ("sequenceRepetition Counter: "));
  debugSerial.println ((uint32_t) sequenceRepetitionCounter);
  debugSerial.print (F ("Task Enabled?: "));
  debugSerial.println (taskEnabled);
  debugSerial.print (F ("Task Suspended?: "));
  debugSerial.println (taskSuspended);
  debugSerial.print (F ("Task Started?: "));
  debugSerial.println (taskStarted);
  debugSerial.print (F ("Cycle Started?: "));
  debugSerial.println (cycleStarted);
  debugSerial.print (F ("Task Running?: "));
  debugSerial.println (taskRunning);
  debugSerial.print (F ("Task Ended?: "));
  debugSerial.println (sequenceRepetitionEnded);
  debugSerial.print (F ("Task Run State: "));
  debugSerial.println (taskRunState);
  debugSerial.print (F ("Input Error: "));
  debugSerial.println (inputError);
  debugSerial.println();
}

//==============================================================================//
/**
 * @brief Activates or enables a task. Only active tasks are executed
 * regardless of their turn for execution.
 * 
 */
void ptScheduler:: enable() {
  taskEnabled = true;
}

//==============================================================================//
/**
 * @brief Returns the task enable state.
 * 
 * @return true Task is enabled.
 * @return false Task is disabled.
 */
bool ptScheduler:: isEnabled() {
  return taskEnabled;
}

//==============================================================================//
/**
 * @brief Suspends a task. The task will not be executed until it is resumed.
 * suspendedIntervalCounter will continue to increment in this mode, which you can
 * monitor to determine how long the task has been suspended and resume the task
 * if necessary.
 * 
 */
void ptScheduler:: suspend() {
  taskSuspended = true;

  // If the execution counter is reset to 0 when suspending a task,
  // the task will start a fresh interval cycle when resumed.
  if (toClearExecutionCounter) {
    executionCounter = 0;
  }
}

//==============================================================================//
/**
 * @brief Returns the suspended state of the task.
 * 
 * @return true Task is suspended.
 * @return false Task is not suspended.
 */
bool ptScheduler:: isSuspended() {
  return taskSuspended;
}

//==============================================================================//
/**
 * @brief Resumes a task from suspended state. The task will resume from where it
 * was suspended.
 * 
 */
void ptScheduler:: resume() {
  taskSuspended = false;
  // intervalCounter = 0;
}

//==============================================================================//
/**
 * @brief Disables a task. This will reset all values to their default states.
 * All user specified values will be preserved, which includes the mode, skip
 * interval, time, sequenceRepetition, default interval, etc. You have to call
 * either enable() or reset() functions to enable the task.
 * 
 */
void ptScheduler:: disable() {
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

//==============================================================================//
/**
 * @brief A reset reinitializes the task.
 * 
 */
void ptScheduler:: reset() {
  disable();
  enable();
}

//==============================================================================//
/**
 * @brief Let's you specify the number of times the interval sequence has to be
 * executed. After the specified number of repetitions, the task will end and sleep
 * either through DISABLE or SUSPEND mode.
 * 
 * @param value The number of sequences to execute.
 * @return true If the task in a valid mode and the repetition is set.
 * @return false If operation failed.
 */
bool ptScheduler:: setSequenceRepetition (int32_t value) {
  switch (taskMode) {
    case PT_MODE_ONESHOT:
      sequenceRepetition = value;
      sequenceRepetitionExtended = value;
      return true;
      break;
    
    case PT_MODE_SPANNING:
      // Determining how many sequenceRepetition have executed for a spanning task with odd no. of intervals is not
      // straight forward as oneshot tasks. Odd number of intervals produce inverted output after every
      // interval set. For example, if the intervals are 1000, 2000 and 3000, the first 1000 ms will be HIGH,
      // next LOW for 2000 ms, then again high for 3000 ms. After that, the pattern is inverted. Since 3000 ms
      // was HIGH, next 1000 ms will be LOW and so on. So counting just the HIGH states (execution counter) won't work.
      // We need to calculate how many intervals we have to check to determine if the specified repetition has
      // completed. But the user has to specify the number of sequenceRepetition only. Therefore we also calculate
      // an additional parameter called sequenceRepetitionExtended which is the number of individual intervals to check.
      sequenceRepetition = value;
      
      // If the sequenceRepetition is 1, then the sequenceRepetitionExtended is the same as the sequence length.
      if (sequenceRepetition == 1) {
        sequenceRepetitionExtended = sequenceLength;
      }
      else if ((sequenceRepetition % 2) == 1) { // If the sequenceRepetition is odd.
        sequenceRepetitionExtended = uint32_t (((sequenceLength + 1) / 2) * sequenceRepetition);
        sequenceRepetitionExtended--; // We need one less, otherwise there will be one extra active state.
      }
      else {
        sequenceRepetitionExtended = uint32_t ((sequenceLength * sequenceRepetition) / 2);
      }
      return true;
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

//==============================================================================//
/**
 * @brief Set the first interval value of an interval sequence.
 * 
 * @param value Time in microseconds.
 * @return true If the value is set.
 * @return false If the interval sequence is empty.
 */
bool ptScheduler:: setInterval (time_us_t value) {
  if (sequenceLength > 0) {
    sequenceList [0] = value;
    return true;
  }
  else {
    inputError = true;
    return false;
  }
}

//==============================================================================//
/**
 * @brief Let's you set the skip duration in terms of number of intervals to skip.
 * The number of intervals to skip can also be greater than the number of intervals
 * in the sequence. In that case, the skip duration will be calculated by summing up
 * the intervals in the sequence and saving to the skipTime variable.
 * 
 * If you set both skip interval and skip iteration, the last call determines the
 * skip duration.
 * 
 * @param value The number of intervals to skip.
 * @return true If the operation is successful.
 * @return false If the sequence is empty.
 */
bool ptScheduler:: setSkipInterval (uint32_t value) {
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

    for (uint32_t i = 0; i < value; i++) {
      if (index == sequenceLength) {
        index = 0;
      }
      skipTime += sequenceList [index]; // Summing up the intervals.
      index++;
    }
    return true;
  }
  return false;
}

//==============================================================================//
/**
 * @brief Let's you specify the skip duration in terms of number of sequences
 * to skip. The actual skip duration will be calculated as a multiple of the
 * interval sequence. Also remember that an interval sequence is a set of intervals.
 * 
 * @param value The number of sequences to skip.
 * @return true If the operation is successful.
 * @return false If the sequence is empty.
 */
bool ptScheduler:: setSkipSequence (uint32_t value) {
  if (sequenceLength > 0) {
    skipSequence = value;

    if (value == 0) {
      skipSequenceSet = false;
      skipTime = 0;
      return true;
    }
    
    skipSequenceSet = true;
    skipTime = 0;

    for (uint8_t i = 0; i < value; i++) {
      for (uint8_t j = 0; j < sequenceLength; j++) {
        skipTime += sequenceList [j];
      }
    }
    return true;
  }
  return false;
}

//==============================================================================//
/**
 * @brief Sets the skip duration in terms of time in microseconds.
 * 
 * @param value Time in microseconds.
 * @return true If the operation is successful.
 * @return false If the sequence is empty.
 */
bool ptScheduler:: setSkipTime (time_us_t value) {
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

//==============================================================================//
/**
 * @brief Checks if there was any input error. Calling this function will
 * also reset the flag.
 * 
 * @return true If there was an input error.
 * @return false If there was no input error.
 */
bool ptScheduler:: isInputError() {
  if (inputError) {
    inputError = false;
    return true;
  }
  return false;
}

//==============================================================================//
