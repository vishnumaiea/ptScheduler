# ptScheduler
**Pretty tiny Scheduler** or **ptScheduler** is a non-preemptive task scheduler library for **Arduino** that helps you write **non-blocking periodic tasks** easily and effectively without using ordinary delay routines or using `millis()` function on your own. You won't have to use **delay()** function again.

Under the hood, ptScheduler uses the native `millis()` implementation. The `millis()` function is a timer based **ISR** that increments a global counter variable (unsigned integer) every millisecond.

When you create a new ptScheduler object, you can specify the time intervals and execution modes. All the class member variables and functions are public and therefore gives you full control over your tasks, allowing dynamically changing the behavior of the task.

To run a task, just enclose the **`call()`** function inside any **conditional statements**, either inside your infinite loop or inside a function. Every time you invoke the `call()` function, it checks the elapsed time to determine if your task has to be executed. If yes, it will return `true` and cause the code under the conditional block to be executed once.

ptScheduler is good mainly for **control applications** with perodic polling of sensors, GPIOs and other parameters.

Head over to the **[Wiki page](https://github.com/vishnumaiea/ptScheduler/wiki)** to learn more.
