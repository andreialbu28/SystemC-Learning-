# SystemC Events — Immediate Notification & the "Event Hog" Problem

This example demonstrates the critical difference between **immediate** and **deferred** `sc_event` notification, and shows how immediate notifications interact with the cooperative kernel to produce a scenario where one pair of threads can completely starve a third.

## Overview

The previous example (`07_SystemC_Events`) used `notify(1, SC_NS)` — deferred notifications that advance simulation time. This example switches to `notify()` with no arguments — **immediate notification** — and reveals an important behavioral consequence: threads communicating via immediate events can loop indefinitely at the same simulation timestamp, preventing any time-based process from ever running.

## The Module

Three threads share two events:

```cpp
sc_event a_event, b_event;

SC_THREAD(A_thread);  // notifies a_event immediately, waits for b_event
SC_THREAD(B_thread);  // notifies b_event immediately, waits for a_event
SC_THREAD(C_thread);  // runs on a 1 ns timer — never gets a turn
```

### `A_thread` and `B_thread` — the event hogs

```cpp
void A_thread() {
    while (true) {
        std::cout << "A @ " << sc_time_stamp() << std::endl;
        a_event.notify();   // immediate — fires THIS delta cycle
        wait(b_event);
    }
}

void B_thread() {
    int count(10);
    while (true) {
        std::cout << "B @ " << sc_time_stamp() << std::endl;
        b_event.notify();   // immediate — fires THIS delta cycle
        wait(a_event);
        if (count-- == 0) sc_stop();
    }
}
```

A and B ping-pong back and forth using immediate notifications. Each time one thread calls `notify()`, it moves the other from the waiting set into the runnable set — but **no simulation time advances**. Both threads keep executing at `0 s`, delta cycle after delta cycle.

### `C_thread` — the starved process

```cpp
void C_thread() {
    while (true) {
        std::cout << "C @ " << sc_time_stamp() << std::endl;
        wait(1, SC_NS);
    }
}
```

`C_thread` uses `wait(1, SC_NS)` — a time-based wait. It will only run again once simulation time advances past its wakeup point. But because A and B keep firing at the same timestamp via immediate notifications, simulation time **never advances**, and `C_thread` never gets another turn after its initialization run.

## Immediate Notification — How It Works

> When a process calls `notify()` (no arguments), it may cause one or more processes in the **waiting set** to be moved into the **runnable set**.

The key word is *may*. The notified processes do not run immediately inline — they are moved to the runnable queue and will be scheduled in the **current delta cycle**, after the currently executing process yields. This means:

```
A runs → calls a_event.notify() → B moves to runnable set
A calls wait(b_event) → A suspends
B runs → calls b_event.notify() → A moves to runnable set
B calls wait(a_event) → B suspends
A runs again → ... (still at 0 s)
```

Because neither thread ever calls a time-advancing `wait()`, the simulation clock never moves. C is stuck waiting for 1 ns that never comes.

## Delta Cycles vs. Simulation Time

| | Delta cycle advance | Simulation time advance |
|---|---|---|
| `notify()` | ✅ Yes | ❌ No |
| `notify(SC_ZERO_TIME)` | ✅ Yes | ❌ No |
| `notify(t, unit)` | ✅ Yes | ✅ Yes (by `t`) |
| `wait(event)` | Yields, resumes same time | No change |
| `wait(t, unit)` | Yields | ✅ Yes (by `t`) |

A and B advance delta cycles indefinitely while holding simulation time fixed at `0 s`. C's `wait(1, SC_NS)` requires simulation time to reach `1 ns`, which never happens.

## Stopping the Simulation

Without a termination condition, this would run forever. Two mechanisms are used together:

```cpp
sc_set_stop_mode(SC_STOP_IMMEDIATE);
```

Set in the constructor, this ensures that when `sc_stop()` is called, the kernel halts **immediately** rather than finishing the current time step. Without this, the default `SC_STOP_FINISH_DELTA` mode would try to complete the current delta — but since A and B never finish their delta, the simulation would still hang.

```cpp
int count(10);
...
if (count-- == 0) sc_stop();
```

B counts down 10 iterations and then calls `sc_stop()`, giving the simulation a guaranteed exit point.

## Expected Output

```
08_01_Ordered_events
C @ 0 s           ← C runs once during initialization
A @ 0 s           ← A and B begin their loop
B @ 0 s
A @ 0 s           ← all at 0 s — time never advances
B @ 0 s
A @ 0 s
B @ 0 s
... (10 iterations)
```

`C` prints exactly once — during the initialization phase before A and B begin their loop. After that, `C` is waiting for `1 ns` that never arrives.

## The "Event Hog" Anti-Pattern

This example is intentionally named `event_hogs` and `bad_use` to flag that this is a pattern to **avoid** in real designs. The problem arises when:

1. Two or more threads communicate exclusively via immediate `notify()`
2. Neither thread ever calls a time-advancing `wait()`
3. Other time-based processes exist in the same simulation

The result is **process starvation** — the time-based process never runs, simulation time freezes, and the design produces incorrect or incomplete results.

The fix is straightforward: use `notify(SC_ZERO_TIME)` or `notify(t, unit)` instead of `notify()` when communicating between threads that should not monopolize the kernel.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `notify()` | Immediate notification — moves waiting processes to runnable in the current delta |
| Delta cycle | A kernel scheduling step within a single simulation timestamp |
| Process starvation | A time-based process that never runs because simulation time never advances |
| `sc_set_stop_mode(SC_STOP_IMMEDIATE)` | Forces `sc_stop()` to halt the kernel immediately, bypassing delta completion |
| `SC_STOP_FINISH_DELTA` | Default stop mode — completes current delta before halting |
| Runnable set / Waiting set | The kernel's two process queues; `notify()` moves processes between them |

## Relation to Previous Examples

The ordered events example (`07_SystemC_Events`) used `notify(1, SC_NS)` which advanced time safely. This example isolates the `notify()` (immediate) form and deliberately constructs a pathological case to show exactly what goes wrong — making this a cautionary companion to example 07 rather than a pattern to follow.

## What Comes Next

With both safe and unsafe event patterns understood, the next topic is **`sc_signal`** — a higher-level communication mechanism that carries typed data between modules and integrates cleanly with the sensitivity system to avoid these scheduling pitfalls.