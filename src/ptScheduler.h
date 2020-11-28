
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

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    int64_t entryTime = 0;  //the entry point of a task, returned by millis()
    int64_t exitTime = 0; //the exit point of a tast, returned by millis()
    int64_t elapsedTime = 0;  //elapsed time since the last task execution
    int64_t residue = 0;
    int64_t interval = 0; //the interval at which the task will be called once 
    int64_t _interval = 0; //backup value of the interval to preserve sign
    uint64_t intervalCounter = 0; //how many intervals has been passed
    uint64_t taskCounter = 0; //how many times the task has been executed
    uint8_t mode = PT_MODE1;
    bool activated = true;  //whether a task is allowed to run or not
    bool taskStarted = false; //whether a task has started an execution cycle
    bool cycleStarted = false; //whether a task has started a time cycle
    bool dormant = false; //if the task is not in running state
    bool suspended = false; //whether a task is prevented from running until further activation
    bool ended = true;  //end of an execution cycle
    bool running = false; //a task is running

    bool skipIntervalSet = false;
    bool skipIterationSet = false;
    bool skipTimeSet = false;
    int64_t skipInterval = 0;
    int64_t skipIteration = 0;
    int64_t skipTime = 0;
    
    ptScheduler (int64_t timeValue); //sets the initial interval for the task
    ~ptScheduler();
    void reset();
    void activate();  //activating a task run at each intervals
    void deactivate();  //prevent a task from running at each intervals
    void suspend();  //prevent a task from running at each intervals
    void resume();  //resume task execution
    bool call();  //the task invokation, determines whether a task should be run or not
    void setInterval (int64_t timeValue);  //dynamically set task interval
    bool setSkipInterval (uint32_t value);
    bool setSkipIteration (uint32_t value);
    bool setSkipTime (int64_t timeValue);
    void setMode (uint8_t _mode);
};

//=======================================================================//
