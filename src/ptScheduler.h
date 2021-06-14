
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.0.0
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 18:19:10 PM 14-06-2021, Monday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//defines

#define debugSerial   Serial

#define PT_MODE_EPO     1   //Equal, Periodic, Oneshot
#define PT_MODE_EIO     2   //Equal, Iterated, Oneshot
#define PT_MODE_UPO     3   //Unequal, Periodic, Oneshot
#define PT_MODE_UIO     4   //Unequal, Iterated, Oneshot
#define PT_MODE_EPS     5   //Equal, Periodic, Spanning
#define PT_MODE_EIS     6   //Equal, Iterated, Spanning
#define PT_MODE_UPS     7   //Unequal, Periodic, Spanning
#define PT_MODE_UIS     8   //Unequal, Iterated, Spanning

#define PT_SLEEP_DISABLE     1    //self-disable mode
#define PT_SLEEP_SUSPEND     2    //self-suspend mode

typedef uint64_t time_ms_t;  //time in milliseconds
typedef uint64_t time_us_t;  //time in milliseconds

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    time_ms_t entryTime = 0;  //the entry point of a task, returned by millis()
    time_ms_t exitTime = 0; //the exit point of a tast, returned by millis()
    time_ms_t elapsedTime = 0;  //elapsed time since the last task execution
    uint64_t intervalCounter = 0; //how many intervals have been passed
    uint64_t sleepIntervalCounter = 0; //how many intervals have been passed after disabling/suspending the task
    uint64_t executionCounter = 0; //how many times the task has been executed
    uint64_t iterationCounter = 0; //how many times the iteration set has been executed
    
    uint32_t iterations = 0;  //how many times a task has to be executed, for each activation
    uint32_t iterationsExtended = 0;  //how many times a task has to be executed, for each activation
    uint8_t taskMode = PT_MODE_EPO;  //the execution mode of a task
    uint8_t sleepMode = PT_SLEEP_DISABLE; //default is disable
    uint8_t intervalCount;  //how many intervals have been passed
    time_ms_t* intervalList;  //a pointer to array of intervals
    uint8_t intervalIndex = 0;  //the position in the interval list
    uint32_t skipInterval = 0;  //number of intervals to skip
    uint32_t skipIteration = 0; //number of iterations to skip
    time_ms_t skipTime = 0; //time to skip before running a task
    uint32_t prevTimeDelta = 0; //previous time difference

    bool taskEnabled = true;  //task is allowed to run or not
    bool taskStarted = false; //task has started an execution cycle
    bool cycleStarted = false; //task has started an interval cycle
    bool taskSuspended = false; //a task is prevented from running until further activation
    bool iterationEnded = false;  //end of an iteration set
    bool taskRunning = false; //a task is running
    bool taskRunState;  //the current execution state of a task

    bool inputError = false;  //if any user input parameters are wrong
    bool skipIntervalSet = false; //if skip interval was set
    bool skipIterationSet = false;  //if skip iteration was set
    bool skipTimeSet = false; //if skip time was set
    
    ptScheduler (time_ms_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_ms_t interval_1);
    ptScheduler (uint8_t _mode, time_ms_t interval_1, time_ms_t interval_2);
    ptScheduler (uint8_t _mode, time_ms_t* listPtr, uint8_t listLength);
    ~ptScheduler();
    void reset(); //disable + enable
    void enable();  //enabling a task to run at each intervals
    void disable();  //block a task from running and reset all state variables and counters
    void suspend();  //block a task from running but without resetting anything. interval counter will still run.
    void resume();  //resume a suspended task
    bool oneshot(); //logic for oneshot tasks
    bool spanning();  //logic for spanning tasks
    bool call();  //the task invokation call
    bool setInterval (time_ms_t value);  //dynamically set task interval
    bool setInterval (time_ms_t value_1, time_ms_t value_2);  //update two interval values. only for tasks instantiated with >= intervals
    bool setIteration (int32_t value);  //no. of iterations you want to execute for each activation
    bool setSkipInterval (uint32_t value);  //intervals to wait before executing the task
    bool setSkipIteration (uint32_t value); //iterations to wait before executing the task
    bool setSkipTime (time_ms_t value); //time to wait before executing the task
    bool setTaskMode (uint8_t mode);  //set execution mode
    bool setSleepMode (uint8_t mode); //set what happens after an iteration is complete
    bool isInputError();
    void printStats();  //prints all the statuses and counter to debug port
    void timeElapsed();
};

//=======================================================================//
