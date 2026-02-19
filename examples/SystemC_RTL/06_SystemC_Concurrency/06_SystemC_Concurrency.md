# SystemC Concurrency

This example demonstrates how SystemC models **concurrent behavior** using multiple `SC_THREAD` processes running within a single module — and explains the cooperative multitasking model that makes it work.

## Overview

Real hardware systems do many things at once. SystemC models this concurrency not with true parallel execution, but with a **cooperative, non-preemptive simulation kernel** that interleaves processes by scheduling them around `wait()` calls. This example uses two threads with different periods to show how the kernel manages that interleaving over time.

## What Is Simulated Concurrency?

In a real system, multiple events can occur at the exact same instant — a car braking, the turn signal flashing, and the engine throttling back all simultaneously. Software, however, typically runs on a single processor and can only do one thing at a time.

SystemC bridges this gap through **cooperative multitasking**:

- Each registered process runs until it voluntarily yields control by calling `wait()`
- The kernel then picks the next process to run based on its schedule
- No process is ever forcibly interrupted — it must cooperate by yielding

> ⚠️ A process that never calls `wait()` — for example, an infinite loop with no yield point — will **hang the simulation**. The kernel cannot preempt it.

This is unlike a typical operating system scheduler, which can interrupt a running thread at any time. In SystemC, every process must be a good citizen and yield regularly.

## The Module

```cpp
SC_MODULE(CONCURRENCY) {
    SC_CTOR(CONCURRENCY) {
        SC_THREAD(thread_1);
        SC_THREAD(thread_2);
    }

    void thread_1(void) {
        while (true) {
            std::cout << sc_time_stamp() << ": thread 1" << std::endl;
            wait(2, SC_SEC);
        }
    }

    void thread_2(void) {
        while (true) {
            std::cout << "     " << sc_time_stamp() << ": thread 2" << std::endl;
            wait(4, SC_SEC);
        }
    }
};
```

Two threads are registered in the constructor. They run independently with different periods:

- `thread_1` wakes every **2 seconds**
- `thread_2` wakes every **4 seconds**

Both start at time zero and follow their own schedule for the 10-second simulation window.

## Execution Timeline

```
Time      thread_1    thread_2
──────────────────────────────
0 s         ✓           ✓
2 s         ✓
4 s         ✓           ✓
6 s         ✓
8 s         ✓           ✓
10 s      (stop)      (stop)
```

At time 0, both threads run their initialization (the code before the first `wait()`). After that, `thread_1` fires every 2 seconds and `thread_2` every 4 seconds. At times 4 s and 8 s they coincide — the kernel runs one then the other within the same simulation time step, with no real time between them.

## Expected Output

```
0 s: thread 1
     0 s: thread 2
2 s: thread 1
4 s: thread 1
     4 s: thread 2
6 s: thread 1
8 s: thread 1
     8 s: thread 2
```

When both threads are scheduled at the same simulation time (0 s, 4 s, 8 s), the kernel runs them sequentially in registration order — `thread_1` first, then `thread_2` — both still showing the same timestamp. This is **delta-cycle** behavior: no simulation time advances between them.

## `SC_THREAD` Registration Rules

`SC_THREAD` has two important restrictions:

1. **Module scope only** — the function must be a member function of a `SC_MODULE` class. It cannot be a free function or a member of a plain C++ class.
2. **Elaboration only** — `SC_THREAD(...)` must be called inside the constructor, during the elaboration phase. Process registration after `sc_start()` is not permitted.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| Simulated concurrency | Multiple processes interleaved by the kernel, not truly parallel |
| Cooperative multitasking | Processes yield voluntarily via `wait()`; the kernel cannot preempt |
| `SC_THREAD` | A suspendable process with its own stack that runs in an infinite loop |
| `wait(time, unit)` | Suspends the thread and yields control to the kernel for the given duration |
| Same-time scheduling | When multiple processes share a timestamp, they run sequentially within a delta cycle |
| Simulation hang | What happens when a process never calls `wait()` — the kernel stalls indefinitely |

## Relation to Previous Examples

The constructor example (`03_SystemC_Constructor`) introduced `SC_THREAD` as one of three process types. This example focuses exclusively on `SC_THREAD` and makes **concurrency** the subject — showing how two independent threads with different rhythms coexist under a single simulation kernel without interfering with each other.

## What Comes Next

With concurrent processes understood, the next step is **signals and events** — giving processes a way to communicate and synchronize with each other rather than simply running on fixed timers.