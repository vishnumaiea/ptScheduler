
//=======================================================================//
/**
 * @file ptScheduler.h
 * @author Vishnu Mohanan (@vishnumaiea)
 * @brief "Pretty tiny Scheduler" is an Arduino library for writing non-blocking
 * periodic tasks without using delay() or millis() routines.
 * 
 * Read the header file for a better understanding of how this library is
 * organized. I have added plenty of comments to help you with it.
 * 
 * @version 2.1.2
 * @link https://github.com/vishnumaiea/ptScheduler
 * @date Last modified : +05:30 16:02:02 PM 29-03-2023, Wednesday
 * @copyright License: MIT
 * 
 */
//=======================================================================//
// Includes

#include <Arduino.h>

//=======================================================================//
// Defines

// Change this to your desired serial port for printing the debug messages
#define debugSerial         Serial

#define  PT_MODE_ONESHOT    1
#define  PT_MODE_SPANNING   2

#define  PT_SLEEP_DISABLE   1    //self-disable mode
#define  PT_SLEEP_SUSPEND   2    //self-suspend mode

#define  PT_MS_MULTIPLIER   1000   //multiplier to convert ms to us

// Time periods
#define  PT_TIME_1MS        1000
#define  PT_TIME_2MS        2000
#define  PT_TIME_5MS        5000
#define  PT_TIME_10MS       10000
#define  PT_TIME_20MS       20000
#define  PT_TIME_50MS       50000
#define  PT_TIME_100MS      100000
#define  PT_TIME_200MS      200000
#define  PT_TIME_500MS      500000
#define  PT_TIME_1S         1000000
#define  PT_TIME_2S         2000000
#define  PT_TIME_3S         3000000
#define  PT_TIME_4S         4000000
#define  PT_TIME_5S         5000000
#define  PT_TIME_6S         6000000
#define  PT_TIME_7S         7000000
#define  PT_TIME_8S         8000000
#define  PT_TIME_9S         9000000
#define  PT_TIME_10S        10000000
#define  PT_TIME_1MIN       60000000  // This line was predicted by AI

// Frequenceis in terms of time values
#define  PT_FREQ_1HZ        PT_TIME_1S
#define  PT_FREQ_2HZ        PT_TIME_500MS
#define  PT_FREQ_5HZ        PT_TIME_200MS
#define  PT_FREQ_10HZ       PT_TIME_100MS
#define  PT_FREQ_20HZ       PT_TIME_50MS
#define  PT_FREQ_50HZ       PT_TIME_20MS
#define  PT_FREQ_100HZ      PT_TIME_10MS
#define  PT_FREQ_200HZ      PT_TIME_5MS
#define  PT_FREQ_500HZ      PT_TIME_2MS
#define  PT_FREQ_1KHZ       PT_TIME_1MS

#define  PT_TIME_DEFAULT    PT_TIME_1S

typedef uint64_t time_ms_t;  // Time in milliseconds
typedef uint64_t time_us_t;  // Time in microseconds

// Add your own timing functions here
#define  GET_MICROS         micros
#define  GET_MILLIS         millis

//=======================================================================//
//main class

class ptScheduler {
  private :
    
  public :
    time_us_t entryTime = 0;  // The entry time of a task, returned by micros()
    time_us_t exitTime = 0; // The exit time of a task, returned by micros()
    time_us_t elapsedTime = 0;  // Elapsed time since entry time
    time_us_t lastElapsedTime = 0;
    uint64_t intervalCounter = 0; // How many intervals have been passed
    uint64_t suspendedIntervalCounter = 0; // How many intervals have been passed after suspending the task
    uint64_t executionCounter = 0; // How many times the task has returned true
    uint64_t sequenceRepetitionCounter = 0; // How many times the sequence has been repeated

    uint32_t sequenceRepetition = 0;  // How many times an interval sequence has to be executed
    uint32_t sequenceRepetitionExtended = 0;  // Repetitions * interval sequence length
    uint8_t taskMode = PT_MODE_ONESHOT;  // The execution mode of the task
    uint8_t sleepMode = PT_SLEEP_DISABLE; // Default is disable
    uint8_t sequenceLength;  // How many intervals in a sequence
    time_us_t* sequenceList;  // A pointer to the interval sequence
    uint8_t sequenceIndex = 0;  // Index position of interval sequence
    uint32_t skipInterval = 0;  // Number of individual intervals to skip
    uint32_t skipSequence = 0; // Number of sequences (set of intervals) to skip
    time_us_t skipTime = 0; // Time to wait before running a task
    uint32_t prevTimeDelta = 0; // Previous time difference
    uint32_t timeDelta = 0; // Current time difference
    uint32_t microsValue = 0; // Value returned by micros()

    bool taskEnabled = true;  // Task is allowed to run or not
    bool taskStarted = false; // Task has started an execution cycle
    bool cycleStarted = false; // Task has started an interval cycle
    bool taskSuspended = false; // A task is prevented from running until further activation
    bool sequenceRepetitionEnded = false;  // End of a repetition
    bool taskRunning = false; // The task is running
    bool taskRunState;  // The current execution state of a task

    bool inputError = false;  // If any user input parameters are wrong
    bool skipIntervalSet = false; // If skip interval was set
    bool skipSequenceSet = false;  // If skip sequence was set
    bool skipTimeSet = false; // If skip time was set
    bool toClearExecutionCounter = false; // If the execution counter has to be cleared

    // Description of all functions can be found in the .cpp file
    ptScheduler (time_us_t interval_1);
    ptScheduler (uint8_t _mode, time_us_t interval_1);
    ptScheduler (uint8_t _mode, time_us_t* listPtr, uint8_t listLength);
    ~ptScheduler();
    void reset();
    void enable();
    void disable();
    void suspend();
    void resume();
    bool oneshot();
    bool spanning();
    bool call();
    bool setInterval (time_us_t value);
    bool setSequenceRepetition (int32_t value);
    bool setSkipInterval (uint32_t value);
    bool setSkipSequence (uint32_t value);
    bool setSkipTime (time_us_t value);
    bool setTaskMode (uint8_t mode);
    bool setSleepMode (uint8_t mode);
    bool isInputError();
    void printStats();
    void getTimeElapsed();
};

//=======================================================================//
