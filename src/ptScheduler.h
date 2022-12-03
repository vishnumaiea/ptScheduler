
//=======================================================================//

// --- Pretty tiny Scheduler ---

// Pretty tiny Scheduler is an Arduino library for writing non-blocking
// periodic tasks without using delay() or millis() routines.

// Author : Vishnu Mohanan (@vishnumaiea)
// Version : 2.1.1
// License : MIT
// Source : https://github.com/vishnumaiea/ptScheduler

// Last modified : +05:30 19:47:49 PM 03-12-2022, Saturday

//=======================================================================//
//includes

#include <Arduino.h>

//=======================================================================//
//defines

//change this to your desired serial port for printing the debug messages
#define debugSerial   Serial

#define PT_MODE_ONESHOT   1
#define PT_MODE_SPANNING  2

#define PT_SLEEP_DISABLE     1    //self-disable mode
#define PT_SLEEP_SUSPEND     2    //self-suspend mode

#define MS_MULTIPLIER  1000   //multiplier to convert ms to us

//time periods
#define PT_TIME_1MS     1000
#define PT_TIME_2MS     2000
#define PT_TIME_5MS     5000
#define PT_TIME_10MS    10000
#define PT_TIME_20MS    20000
#define PT_TIME_50MS    50000
#define PT_TIME_100MS   100000
#define PT_TIME_200MS   200000
#define PT_TIME_500MS   500000
#define PT_TIME_1S      1000000
#define PT_TIME_2S      2000000
#define PT_TIME_3S      3000000
#define PT_TIME_4S      4000000
#define PT_TIME_5S      5000000
#define PT_TIME_6S      6000000
#define PT_TIME_7S      7000000
#define PT_TIME_8S      8000000
#define PT_TIME_9S      9000000
#define PT_TIME_10S     10000000
#define PT_TIME_1MIN    60000000  //this line was predicted by AI

//equivalent frequenceis
#define PT_FREQ_1HZ       PT_TIME_1S
#define PT_FREQ_2HZ       PT_TIME_500MS
#define PT_FREQ_5HZ       PT_TIME_200MS
#define PT_FREQ_10HZ      PT_TIME_100MS
#define PT_FREQ_20HZ      PT_TIME_50MS
#define PT_FREQ_50HZ      PT_TIME_20MS
#define PT_FREQ_100HZ     PT_TIME_10MS
#define PT_FREQ_200HZ     PT_TIME_5MS
#define PT_FREQ_500HZ     PT_TIME_2MS
#define PT_FREQ_1KHZ      PT_TIME_1MS

#define PT_TIME_DEFAULT   PT_TIME_1S

typedef uint64_t time_ms_t;  //time in milliseconds
typedef uint64_t time_us_t;  //time in microseconds

//add your own timing functions here
#define GET_MICROS micros
#define GET_MILLIS millis

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    time_us_t entryTime = 0;  //the entry point of a task, returned by micros()
    time_us_t exitTime = 0; //the exit point of a task, returned by micros()
    time_us_t elapsedTime = 0;  //elapsed time since entry time
    time_us_t lastElapsedTime = 0;
    uint64_t intervalCounter = 0; //how many intervals have been passed
    uint64_t suspendedIntervalCounter = 0; //how many intervals have been passed after suspending the task
    uint64_t executionCounter = 0; //how many times the task has returned true
    uint64_t sequenceRepetitionCounter = 0; //how many times the sequence has been repeated

    uint32_t sequenceRepetition = 0;  //how many times an interval sequence has to be executed
    uint32_t sequenceRepetitionExtended = 0;  //repetitions * interval sequence length
    uint8_t taskMode = PT_MODE_ONESHOT;  //the execution mode of the task
    uint8_t sleepMode = PT_SLEEP_DISABLE; //default is disable
    uint8_t sequenceLength;  //how many intervals in a sequence
    time_us_t* sequenceList;  //a pointer to the interval sequence
    uint8_t sequenceIndex = 0;  //index position of interval sequence
    uint32_t skipInterval = 0;  //number of individual intervals to skip
    uint32_t skipSequence = 0; //number of sequences (set of intervals) to skip
    time_us_t skipTime = 0; //time to wait before running a task
    uint32_t prevTimeDelta = 0; //previous time difference
    uint32_t timeDelta = 0; //time difference
    uint32_t microsValue = 0; //value returned by micros()

    bool taskEnabled = true;  //task is allowed to run or not
    bool taskStarted = false; //task has started an execution cycle
    bool cycleStarted = false; //task has started an interval cycle
    bool taskSuspended = false; //a task is prevented from running until further activation
    bool sequenceRepetitionEnded = false;  //end of a repetition
    bool taskRunning = false; //a task is running
    bool taskRunState;  //the current execution state of a task

    bool inputError = false;  //if any user input parameters are wrong
    bool skipIntervalSet = false; //if skip interval was set
    bool skipSequenceSet = false;  //if skip sequence was set
    bool skipTimeSet = false; //if skip time was set
    bool toClearExecutionCounter = false; // if execution counter has to be cleared

    ptScheduler (time_us_t interval_1); //sets the initial interval for the task
    ptScheduler (uint8_t _mode, time_us_t interval_1);
    ptScheduler (uint8_t _mode, time_us_t* listPtr, uint8_t listLength);
    ~ptScheduler();
    void reset(); //disable + enable
    void enable();  //enabling a task to run at each intervals
    void disable();  //block a task from running and reset all state variables and counters
    void suspend();  //block a task from running but without resetting anything. interval counter will still run.
    void resume();  //resume a suspended task
    bool oneshot(); //logic for oneshot tasks
    bool spanning();  //logic for spanning tasks
    bool call();  //the task invocation call
    bool setInterval (time_us_t value);  //dynamically set task interval
    bool setSequenceRepetition (int32_t value);  //no. of sequences you want to execute for each activation
    bool setSkipInterval (uint32_t value);  //intervals to wait before executing the task
    bool setSkipSequence (uint32_t value); //number of sequence to wait before executing the task
    bool setSkipTime (time_us_t value); //time to wait before executing the task
    bool setTaskMode (uint8_t mode);  //set execution mode
    bool setSleepMode (uint8_t mode); //set what happens after a repetition is complete
    bool isInputError();  //check if any user inputs were invalid.
    void printStats();  //prints all the statuses and counter to debug port
    void getTimeElapsed();
};

//=======================================================================//
