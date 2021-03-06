<img src="https://github.com/vishnumaiea/ptScheduler/blob/main/assets/ptScheduler-Feature-Image-1_3-1.png" alt="ptScheduler" width="500"/>

# Pretty tiny Scheduler
**Pretty tiny Scheduler** or **ptScheduler** is a non-preemptive task scheduler library for **Arduino** that helps you to write **non-blocking periodic tasks** easily and effectively without using ordinary delay routines or using `millis()` function on your own.

You won't have to use **delay()** or **millis()** functions again.

Under the hood, ptScheduler uses the native `millis()` implementation. The `millis()` function is a hardware timer based **ISR** that increments a global counter variable (unsigned integer) every millisecond.

When you create a new ptScheduler object, you can specify the **time intervals** and **execution modes**. All the class member variables and functions are public and therefore gives you full control over your tasks, allowing dynamically changing the behavior of the task.

To run a task, just enclose the **`call()`** function inside any **conditional statements**, either inside your infinite loop or inside a function. Every time you invoke the `call()` function, it checks if the elapsed time is larger than the preset interval. If yes, it will return `true` and cause the code under the conditional block to be executed once.

ptScheduler is good mainly for **control applications** that require the periodic polling of sensors, GPIOs and other IO devices. ptScheduler tasks can coexist with preemptive tasks such as **FreeRTOS** tasks.

## Hello World

Here is the basic `Hello World` example.

```cpp
#include "ptScheduler.h"

//create tasks
ptScheduler sayHello = ptScheduler(1000);

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

## Tutorial and Documentation

Complete tutorial can be found at **[CIRCUITSTATE](https://circuitstate.com/tutorials/ptscheduler-a-minimal-cooperative-task-scheduler-for-arduino/)**

## Reference

See the reference of all available functions and variables in the **[Reference](https://github.com/vishnumaiea/ptScheduler/wiki/Reference)** page.
