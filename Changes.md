

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

  I have to implement a new "counter only" mode. It will simply increment a counter (`intervalCounter`) without any other overheads.

#

**+05:30 09:20:43 PM 06-12-2020, Sunday**

  Changed `time_ms_t` type to `uint64_t` from `int64_t`. This is because the `millis()` function returns unsigned value always, and we do not use negative interval values. So why waste that extra half? Also, I just came to notice that my code is already overflow safe. For example looks the code below,

    elapsedTime = millis() - entryTime;

  Assume `millis()` was overflown and the current value is now 100. Entry time was saved before the overflow event. So it should have a large value. Let it be 4294967280 which is 15 less than the max value of a 32-bit unsigned value which is 4294967295. So the result would be 184 which is actually the correct time elapsed. Why 184 but not 185 is because one increment was taken for incrementing to 0.

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
