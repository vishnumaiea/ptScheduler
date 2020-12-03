
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.9
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 01:12:00 AM 03-12-2020, Thursday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//defines

#define debugSerial   Serial

//task execution modes
#define PT_MODE0    0   //reserved mode
#define PT_MODE1    1   //Periodic Oneshot
#define PT_MODE2    2   //Iterated Oneshot
#define PT_MODE3    3   //Periodic Equal Spanning
#define PT_MODE4    4   //Iterated Equal Spanning
#define PT_MODE5    5   //Periodic Unequal Spanning
#define PT_MODE6    6   //Iterated Unequal Spanning

//task sleep modes
#define PT_SLEEP_MODE1     1    //self-deactivate mode
#define PT_SLEEP_MODE2     2    //self-suspend mode

typedef int64_t time_ms_t;  //time in milliseconds

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    time_ms_t entryTime = 0;  //the entry point of a task, returned by millis()
    time_ms_t exitTime = 0; //the exit point of a tast, returned by millis()
    time_ms_t elapsedTime = 0;  //elapsed time since the last task execution
    time_ms_t residualTime = 0; //elapsed time - interval time
    uint64_t intervalCounter = 0; //how many intervals have been passed
    uint64_t sleepIntervalCounter = 0; //how many intervals have been passed after deactivating/suspending the task
    uint64_t executionCounter = 0; //how many times the task has been executed
    uint64_t iterationCounter = 0; //how many times the iteration set has been executed
    
    uint32_t iterations = 0;  //how many times a task has to be executed, for each activation
    uint8_t taskMode = PT_MODE1;  //the execution mode of a task
    uint8_t sleepMode = PT_SLEEP_MODE1; //default is deactivation
    uint8_t intervalCount;  //how many intervals have been passed
    time_ms_t* intervalList;  //a pointer to array of intervals
    uint8_t intervalIndex = 0;  //the position in the interval list
    time_ms_t interval_s = 0; //backup signed value of first interval
    uint32_t skipInterval = 0;  //number of intervals to skip
    uint32_t skipIteration = 0; //number of iterations to skip
    time_ms_t skipTime = 0; //time to skip before running a task

    bool activated = true;  //a task is allowed to run or not
    bool taskStarted = false; //a task has started an execution cycle
    bool cycleStarted = false; //a task has started an interval cycle
    bool dormant = false; //if the task is not in running state
    bool suspended = false; //a task is prevented from running until further activation
    bool iterationEnded = false;  //end of an iteration set
    bool running = false; //a task is running
    bool runState;  //the current execution state of a task

    bool inputError = false;  //if any user input parameters are wrong
    bool skipIntervalSet = false; //if skip interval was set
    bool skipIterationSet = false;  //if skip iteration was set
    bool skipTimeSet = false; //if skip time was set
    
    ptScheduler (time_ms_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_ms_t interval_1);
    ptScheduler (uint8_t _mode, time_ms_t interval_1, time_ms_t interval_2);
    ptScheduler (uint8_t _mode, time_ms_t* listPtr, uint8_t listLength);
    ~ptScheduler();
    void reset(); //deactivate + activate
    void activate();  //activating a task to run at each intervals
    void deactivate();  //prevent a task from running and reset all state variables and counters
    void suspend();  //prevent a task from running at each intervals but without resetting anything
    void resume();  //resume a suspended task
    bool call();  //the task invokation call
    bool setInterval (time_ms_t value);  //dynamically set task interval
    bool setInterval (time_ms_t value_1, time_ms_t value_2);  //update two interval values. only for tasks instantiated with >= intervals
    bool setIteration (int32_t value);  //no. of iterations you want to execute for each activation
    bool setSkipInterval (uint32_t value);  //intervals to wait before executing the task
    bool setSkipIteration (uint32_t value); //iterations to wait before executing the task
    bool setSkipTime (time_ms_t value); //time to wait before executing the task
    bool setTaskMode (uint8_t mode);  //set execution mode
    bool setSleepMode (uint8_t mode); //set what happens after an iteration is complete
    void printStats();  //prints all the statuses and counter to debug port
};

//=======================================================================//
