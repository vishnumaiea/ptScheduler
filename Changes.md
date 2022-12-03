

#
**+05:30 08:09:52 PM 03-12-2022, Saturday**

  * Added new variable `toClearExecutionCounter` to control whether or not to reset the `executionCounter` when a task is suspended automatically or manually. Both behaviors may be useful in some cases. The default value is `false`.

#
**+05:30 10:43:12 PM 10-08-2022, Wednesday**

  🆕 New version **2.1.1**

#

**+05:30 10:24:50 PM 02-07-2022, Saturday**

  There was a bug 🪲 at the `setSequenceRepetition()` function. When the `sequenceRepetition` was 1, the `sequenceRepetitionExtended` would become 0 and thus a repeated task would run continuously. It was fixed by adding a condition to check if the `sequenceRepetition` is 1. Also I was using `sequenceLength` for the conditional check where it should have been odd checking on `sequenceRepetition` instead. Both are fixed now. FIXED ✅ [Issue #5](https://github.com/vishnumaiea/ptScheduler/issues/5) .

  ```cpp
  if (sequenceRepetition == 1) {
    sequenceRepetitionExtended = 1;
  }
  else if ((sequenceRepetition % 2) == 1) {
    sequenceRepetitionExtended = uint32_t(((sequenceLength + 1) / 2) * sequenceRepetition);
    sequenceRepetitionExtended--; //we need one less, otherwise there will be one extra active state
  }
  else {
    sequenceRepetitionExtended = uint32_t((sequenceLength * sequenceRepetition) / 2);
  }
  ```
#

**+05:30 11:07:41 AM 29-03-2022, Tuesday**

  I have renamed a few of the variables to better match their job.

  There is also going to be changes in the type of tasks. We will only have **ONESHOT** or **SPANNING** tasks. Everything else about a task can be customized.


#

**+05:30 05:32:14 PM 27-09-2021, Monday**

  `taskMode` has been renamed to `executionMode` because it makes more sense what the parameter controls.


#

**+05:30 05:04:34 PM 31-08-2021, Tuesday**

  I am going to rename Periodic tasks as "Infinite" and Iterated tasks as "Finite". Because both Periodic and Iterated are confusing and unintuitive. With these renaming, it will now be more clear. They can be described as "Finite Repetition Mode" and "Infinite Repetition Mode".

  Terms Oneshot and Spanning will be kept, since they are already meaningful.

  Now there can only be two type of tasks; Oneshot and Spanning with two modes; Finite and Infinite.

  Timing modes = Oneshot and Spanning
  Repetition modes = Finite and Infinite

  Each task can be given with an interval sequence. This is a set of intervals that is always greater than 0.

#

**+05:30 06:19:25 PM 14-06-2021, Monday**

  Let's commit this version as **Version 2.0.0** since this will be incompatible with the previous version.

#

**+05:30 03:50:27 PM 14-06-2021, Monday**

  Added `millis()` overflow workarounds. This is accomplished with a new function `timeElapsed()`. I also replaced all `millis()` instances with `micros()`.

  It is working now, but for some reason the `Hello-World.ino` prints "Hello World" twice every second! 😅

  Okay I found it. It was necessary to reset `prevTimeDelta` along with `elapsedTime` at the point of entry. Now it works flawlessly 🤍

#

**+05:30 11:59:55 AM 13-06-2021, Sunday**

  Ready for a next release with important bug fixes. **Version 1.1.2** 🎉

#

**+05:30 03:42:00 PM 11-06-2021, Friday**

  When does a task start? - A task starts when the first `call()` is made 📋

  When a task is resumed, should it respect the skip parameters? Currently, this is not respected. But what if I want to have that feature? One method is to disable the task and later enable it, instead of suspending and resuming. But this also causes every counter to freeze and thus you won't be able to read the sleep interval counters. You will have to manually and arbitrarily enable the task. But this assures that the skip parameters are respected.

  Another method is to keep the task sleep mode to `PT_SLEEP_SUSPEND` and call `resume()` to the resume the task. But just before calling `resume()`, set `taskStarted` state variable to `false`. This is because it is the `taskStarted` flag that determines if an interval has to be skipped. This flag is set to `true` when you invoke `call()` for the first time (after a duration that is greater than the set skip interval).

  So when we do the following for example, the skip intervals are respected.

  ```cpp
    eioTask.taskStarted = false;
    eioTask.resume();
  ```
#

**+05:30 09:46:25 PM 10-06-2021, Thursday**

  Found a bug 🐞 in the `oneshot()` and `spanning()` routines (lines 261 and 364). When checking for skip parameters, the conditional statement had two instances of `skipIntervalSet`. One should have been `skipIterationSet`. Due to this, `setSkipIteration()` was not working for iterated tasks. FIXED ✅ [Issue #2](https://github.com/vishnumaiea/ptScheduler/issues/2#issue-919728782)

#

**+05:30 04:16:52 PM 10-06-2021, Thursday**

  I plan to optimize the interface of `ptScheduler` by removing the redundant modes. There only needs to be four things to consider when defining a task; oneshot or spanning, and periodic or iterated. That yields 4 modes. Every other modes can be implemented using these four base properties. TODO ⌛

  Second improvement is to make `ptScheduler` use **microsecond timing**. This is already implemented in Arduino platform through `micros()` routine which should be based on a timer interrupt. This will require a new type to be defined, `time_us_t` which will still be a `uint64_t`. TODO ⌛

  Third improvement is about spanning tasks. Currently, the `call()` returns `false` as soon as a single interval is elapsed. Any code block under the call would abruptly stop. Instead of such an abrupt stop, we could signal the code block that the time has elapsed or is about to elapse, and the next call would return false. The task then will be able to properly finish what it was doing. I am not sure if a variable already exists that can signal this state. But once the other two improvements are done, I will try to implement this. TODO ⌛

  For now, we will have a commit of **Version 1.1.1** with some improvements in the code readability and organization. No changes in functionality 🎉

#

**+05:30 03:16:53 PM 10-06-2021, Thursday**

  To keep the `call()` function simple, I have moved the logic of both spanning and oneshot tasks to their own functions, `oneshot()` and `spanning()`.

#

**+05:30 12:39:54 AM 08-12-2020, Tuesday**

  I have to implement a new "counter only" mode. It will simply increment a counter (`intervalCounter`) without any other overheads 💡

#

**+05:30 09:20:43 PM 06-12-2020, Sunday**

  Changed `time_ms_t` type to `uint64_t` from `int64_t`. This is because the `millis()` function returns unsigned value always, and we do not use negative interval values. So why waste that extra half? Also, I just came to notice that the code is already overflow safe. For example look at the code below,

    elapsedTime = millis() - entryTime;

  Assume `millis()` was overflown and the current value is now 100. Entry time was saved before the overflow event. So it should have a large value. Let it be 4294967280 which is 15 less than the max value of a 32-bit unsigned value which is 4294967295. So the result would be 184, which is actually the correct time elapsed. Why 184 but not 185 is because one increment was taken for incrementing to 0.

#

**+05:30 10:28:57 PM 05-12-2020, Saturday**

  **ptScheduler** is ready for final release. There is a many redundant code in this version. I need to optimize the code for a lower code/flash size and RAM usage, without affecting the front-end interface. 

#

**+05:30 08:43:41 PM 04-12-2020, Friday**

  I don't think we need to negative values for intervals. The original intention was that it could allow us to skip the first interval. But with new modes and dedicated time skipping functions, I think the original feature is now redundant. Let's remove signs of all intervals by getting the `abs()` and delete the `interval_s` variable.

#

**+05:30 11:51:49 PM 02-12-2020, Wednesday**

  I've just reduced the code size from 25% to 22%. The code for modes 3, 4, 5 and 6 are same. So `ptScheduler` is almost complete! Yay 🎉

#

**+05:30 11:42:47 PM 02-12-2020, Wednesday**

  My initial assumption that unequal interval could only apply to spanning tasks was not correct. It could also apply to oneshot tasks. Someone someday may find it useful.

  Btw, this is the development log for this project where I speak stupidity and rant in solitude. New entries are added from the top of this file, a trick I INVENTED so that I don't have to scroll to the bottom of a big ass file just to rant. Who is reading this anyway!

  I am not done talking yet. You have to add three new lines before adding an entry + a sensibly formatted (yes Americans, that's you) timestamp. And don't forget to indent your text, so that smart text editors like VS Code will be able to fold large chunks of text for you.
