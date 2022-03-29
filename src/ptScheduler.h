
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 12:46:30 PM 02-02-2022, Wednesday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//defines

#define debugSerial   Serial

// #define PT_TMODE_O  1   //Timing mode = Oneshot (default)
// #define PT_TMODE_S  2   //Timing mode = Spanning
// #define PT_RMODE_I  3   //Repetition mode = Infinite (default)
// #define PT_RMODE_F  4   //Repetition mode = Finite

#define PT_MODE_OI  1
#define PT_MODE_OF  2
#define PT_MODE_SI  3
#define PT_MODE_SF  4

#define PT_SLEEP_DISABLE     1    //self-disable mode
#define PT_SLEEP_SUSPEND     2    //self-suspend mode

#define MS_MULTIPLIER  1000   //multiplier to convert ms to us

typedef uint64_t time_ms_t;  //time in milliseconds
typedef uint64_t time_us_t;  //time in microseconds

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    time_us_t entryTime = 0;  //the entry point of a task, returned by micros()
    time_us_t exitTime = 0; //the exit point of a tast, returned by micros()
    time_us_t elapsedTime = 0;  //elapsed time since the last task execution
    time_us_t lastElapsedTime = 0;  //this is the last calculated elapsed time that is not reset
    uint64_t intervalCounter = 0; //how many intervals have been passed
    uint64_t suspendedIntervalCounter = 0; //how many intervals have been passed after disabling/suspending the task
    uint64_t executionCounter = 0; //how many times the task has returned true
    uint64_t repetetionsCounter = 0; //how many times the sequence has been repeated
    
    uint32_t repetitions = 0;  //how many times an interval sequence has to be executed
    uint32_t repetitionsExtended = 0;  //repetitions * interval sequence length
    uint8_t taskMode = PT_MODE_OI;  //the execution mode of the task
    uint8_t sleepMode = PT_SLEEP_DISABLE; //default is disable
    uint8_t intervalSequenceLength;  //how many intervals in a sequence
    time_us_t* intervalSequence;  //a pointer to the interval sequence
    uint8_t intervalSequenceIndex = 0;  //index position of interval sequence
    uint32_t skipInterval = 0;  //number of individual intervals to skip
    uint32_t skipSequence = 0; //number of sequences (set of intervals) to skip
    time_us_t skipTime = 0; //time to wait before running a task
    uint32_t prevTimeDelta = 0; //previous time difference

    bool taskEnabled = true;  //task is allowed to run or not
    bool taskStarted = false; //task has started an execution cycle
    bool cycleStarted = false; //task has started an interval cycle
    bool taskSuspended = false; //a task is prevented from running until further activation
    bool repetitionsEnded = false;  //end of a repetition
    bool taskRunning = false; //a task is running
    bool taskRunState;  //the current execution state of a task

    bool inputError = false;  //if any user input parameters are wrong
    bool skipIntervalSet = false; //if skip interval was set
    bool skipSequenceSet = false;  //if skip sequence was set
    bool skipTimeSet = false; //if skip time was set
    
    ptScheduler (time_us_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_us_t interval_1);
    // ptScheduler (uint8_t _mode, time_us_t interval_1, time_us_t interval_2);
    ptScheduler (uint8_t _mode, time_us_t* listPtr, uint8_t listLength);
    ~ptScheduler();
    void reset(); //disable + enable
    void enable();  //enabling a task to run at each intervals
    void disable();  //block a task from running and reset all state variables and counters
    void suspend();  //block a task from running but without resetting anything. interval counter will still run.
    void resume();  //resume a suspended task
    bool oneshot(); //logic for oneshot tasks
    bool spanning();  //logic for spanning tasks
    bool call();  //the task invokation call
    bool setInterval (time_us_t value);  //dynamically set task interval
    bool setInterval (time_us_t value_1, time_us_t value_2);  //update two interval values. only for tasks instantiated with >= intervals
    bool setRepetition (int32_t value);  //no. of sequences you want to execute for each activation
    bool setSkipInterval (uint32_t value);  //intervals to wait before executing the task
    bool setSkipSequence (uint32_t value); //number of sequence to wait before executing the task
    bool setSkipTime (time_us_t value); //time to wait before executing the task
    bool setTaskMode (uint8_t mode);  //set execution mode
    bool setSleepMode (uint8_t mode); //set what happens after an repetition is complete
    bool isInputError();
    void printStats();  //prints all the statuses and counter to debug port
    void timeElapsed();
};

//=======================================================================//
