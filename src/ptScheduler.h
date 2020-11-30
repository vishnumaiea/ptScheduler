
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.3
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 12:18:08 AM 29-11-2020, Sunday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//defines

#define debugSerial   Serial

//task execution modes
#define PT_MODE0    0
#define PT_MODE1    1   //Periodic Oneshot
#define PT_MODE2    2   //Iterated Oneshot
#define PT_MODE3    3   //Periodic Equal Spanning
#define PT_MODE4    4   //Iterated Equal Spanning
#define PT_MODE5    5   //Periodic Unequal Spanning
#define PT_MODE6    6   //Iterated Unequal Spanning
#define PT_MODE7    7   
#define PT_MODE8    8
#define PT_MODE9    9

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
    time_ms_t residualTime = 0;
    uint64_t intervalCounter = 0; //how many intervals has been passed
    uint64_t sleepIntervalCounter = 0; //how many intervals have been passed after deactivating/suspending the task
    uint64_t executionCounter = 0; //how many times the task has been executed
    uint64_t iterationCounter = 0; //how many times iteration set has been executed
    
    uint32_t iterations = 0;  //how many times a task has to be executed for each activation
    uint8_t taskMode = PT_MODE1;
    uint8_t sleepMode = PT_SLEEP_MODE1;
    uint8_t intervalCount;
    time_ms_t* intervalList;
    time_ms_t interval_s = 0; //backup signed value of first interval
    uint32_t skipInterval = 0;
    uint32_t skipIteration = 0;
    time_ms_t skipTime = 0;

    bool activated = true;  //whether a task is allowed to run or not
    bool taskStarted = false; //whether a task has started an execution cycle
    bool cycleStarted = false; //whether a task has started a time cycle
    bool dormant = false; //if the task is not in running state
    bool suspended = false; //whether a task is prevented from running until further activation
    bool iterationEnded = true;  //end of an execution cycle
    bool running = false; //a task is running
    bool runState;

    bool inputError = false;  //if any user input parameters are wrong
    bool skipIntervalSet = false;
    bool skipIterationSet = false;
    bool skipTimeSet = false;
    
    
    ptScheduler (time_ms_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_ms_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_ms_t interval_1, time_ms_t interval_2); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_ms_t* listPtr, uint8_t listLength); //sets the initial interval for the task
    ~ptScheduler();
    void reset();
    void activate();  //activating a task run at each intervals
    void deactivate();  //prevent a task from running at each intervals
    void suspend();  //prevent a task from running at each intervals
    void resume();  //resume task execution
    bool call();  //the task invokation, determines whether a task should be run or not
    bool setInterval (time_ms_t value);  //dynamically set task interval
    bool setInterval (time_ms_t value_1, time_ms_t value_2);
    bool setIteration (int32_t value);
    bool setSkipInterval (uint32_t value);
    bool setSkipIteration (uint32_t value);
    bool setSkipTime (time_ms_t value);
    bool setTaskMode (uint8_t mode);
    bool setSleepMode (uint8_t mode);
    void printStats();
};

//=======================================================================//
