<img src="https://github.com/vishnumaiea/ptScheduler/blob/main/assets/ptScheduler-Feature-Image-1_3-1.png" alt="ptScheduler" width="500"/>

# Pretty tiny Scheduler
**Pretty tiny Scheduler** or **ptScheduler** is a non-preemptive task scheduler and timing library for **Arduino**. It helps you write **non-blocking, periodic tasks** easily without using ordinary delay routines or using `millis()` or `micros()` functions. Delay routines can prevent other parts of your code from running until the delay is exhausted. Using `millis()` function in your code can make it harder to understand and manage. ptScheduler solves these problems.

ptScheduler acts as a wrapper for `millis()` or `micros()` based logic used for timing of tasks. Under the hood, ptScheduler uses the native `micros()` implementation of Arduino. The `micros()` function is a hardware timer-based **ISR** that increments a global counter variable (unsigned integer) every microsecond. This function is available on all microcontrollers that support the Arduino framework. ptScheduler was using `millis()` function previously, but the new `micros()` provides granular timing in microseconds compared to milliseconds.

When you create a new ptScheduler task, you can specify the **time intervals** and **execution modes**. There are two execution modes - **ONESHOT** and **SPANNING**. Oneshot task is executed only once every time an interval is elapsed. Spanning task on the other hand remains active during the span of an interval.

All class member variables and functions are public and therefore give you full control over your tasks, allowing dynamically changing the behavior of the task.
To run a task, just enclose the `call()` function inside any conditional statements, either inside your infinite loop or inside a function. Every time you invoke the `call()` function, it checks if the elapsed time is larger than the preset interval. If yes, it will return `true` and cause the code under the conditional block to be executed.

ptScheduler is good mainly for **control applications** that require periodic polling of sensors, GPIOs, and other IO devices. ptScheduler tasks can coexist with preemptive tasks such as **FreeRTOS** tasks.

## Hello World

Here is the basic `Hello World` example.

```cpp
#include "ptScheduler.h"

//create tasks
ptScheduler sayHello = ptScheduler(1000000); //time in microseconds

//setup function runs once
void setup() {
  Serial.begin(9600);
}

//infinite loop
void loop() {
  if (sayHello.call()) {  //executed every second
    Serial.println("Hello World");
  }

  //add other tasks and non-blocking code here
}
```

## Tutorial

Complete tutorial can be found at **[CIRCUITSTATE](https://circuitstate.com/tutorials/ptscheduler-a-minimal-cooperative-task-scheduler-for-arduino/)**

## Documentation

See the reference of all available functions and variables in the **[Documentation](https://vishnumaiea.gitbook.io/ptscheduler/)** page.
