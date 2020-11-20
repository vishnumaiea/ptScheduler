
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is a small library for writing non-blockcing
// periodic tasks for Arduino without using traditional NOP delay routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 0.0.2
// License : MIT
// Repo : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 10:29:34 PM 13-11-2020, Friday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    int64_t entry = 0;  //the entry point of a task, returned by millis()
    int64_t exit = 0; //the exit point of a tast, returned by millis()
    int64_t elapsed = 0;  //elapsed time since the last task execution
    int64_t residue = 0;
    int64_t interval = 0; //the interval at which the task will be called once 
    int64_t interval_ = 0; //backup value of the interval
    uint64_t intervalCounter = 0; //how many intervals has been passed
    uint64_t taskCounter = 0; //how many times the task has been executed
    bool activated = true;  //whether a task is allowed to run or not
    bool started = false; //whether a task has started an execution cycle
    bool dormant = false; //if the task is not in running state
    bool suspended = false; //whether a task is prevented from running until further activation
    bool ended = true;  //end of an execution cycle
    bool running = false; //a task is running
    
    ptScheduler(int64_t timeValue); //sets the initial interval for the task
    ~ptScheduler();
    void activate();  //activating a task run at each intervals
    void deactivate();  //prevent a task from running at each intervals
    void suspend();  //prevent a task from running at each intervals
    bool call();  //the task invokation, determines whether a task should be run or not
    void setInterval(int64_t timeValue);  //dynamically set task interval
};

//=======================================================================//
