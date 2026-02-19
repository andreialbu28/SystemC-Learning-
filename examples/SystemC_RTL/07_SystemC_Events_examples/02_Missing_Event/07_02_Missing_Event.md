# SystemC Events — The Missing Event Problem

This example demonstrates a subtle but critical pitfall with `sc_event`: **a notification that fires before any process is waiting on it is permanently lost**. It also introduces `notify(SC_ZERO_TIME)` — delta-delayed notification — as a safe alternative to immediate `notify()` for inter-thread signaling.

## Overview

`sc_event` has no memory. If a thread notifies an event and no other thread is currently blocked on `wait(event)`, the notification vanishes silently — the waiting threads that arrive later will block forever. This example constructs exactly that scenario and shows the one case where it actually works safely: using `SC_ZERO_TIME` to defer the notification by a single delta cycle, giving waiting threads a chance to register first.

## Project Structure

```
├── include/
│   └── missing_event.h       # Module declaration
├── src/
│   ├── missing_event.cpp     # Thread implementations
│   └── main.cpp              # sc_main entry point
```

## The Module

Three threads and one event:

```cpp
sc_event a_event;

SC_THREAD(B_thread);   // waits for a_event
SC_THREAD(A_thread);   // notifies a_event
SC_THREAD(C_thread);   // also waits for a_event
```

```cpp
void A_thread() {
    a_event.notify(SC_ZERO_TIME);      // delta notification — safe
    cout << "A scheduled a_event (delta)!" << endl;
}

void B_thread() {
    wait(a_event);
    cout << "B got a_event!" << endl;
}

void C_thread() {
    wait(a_event);
    cout << "C got a_event!" << endl;
}
```

None of these threads have an infinite loop — each runs once and exits. `A_thread` fires the event and terminates; `B_thread` and `C_thread` each wait for the event and terminate once they receive it.

## Why Registration Order Matters Here

The threads are registered in this order:

```cpp
SC_THREAD(B_thread);
SC_THREAD(A_thread);
SC_THREAD(C_thread);
```

During initialization (the delta-0 phase before any time advances), the kernel runs threads in registration order. So the sequence is:

```
Delta 0:
  B_thread runs → calls wait(a_event) → suspends immediately
  A_thread runs → calls notify(SC_ZERO_TIME) → schedules notification for delta 1
                  prints "A scheduled a_event (delta)!" → exits
  C_thread runs → calls wait(a_event) → suspends

Delta 1:
  a_event fires → B and C both move to runnable set
  B_thread resumes → prints "B got a_event!" → exits
  C_thread resumes → prints "C got a_event!" → exits
```

Both B and C are already in the waiting set by the time the notification arrives in delta 1. The event reaches both of them.

## The Missing Event Scenario — What Goes Wrong Without `SC_ZERO_TIME`

If `A_thread` called `notify()` (immediate) instead of `notify(SC_ZERO_TIME)`:

```
Delta 0:
  B_thread runs → wait(a_event) → suspends ✓
  A_thread runs → notify() fires RIGHT NOW in delta 0
                  → B is in the waiting set, B wakes up ✓
                  → C has not run yet — it is not waiting yet ✗
  C_thread runs → wait(a_event) → suspends... forever ✗
```

C misses the event entirely because it had not yet reached its `wait()` call when A fired the immediate notification. C will block for the full 20 seconds of simulation time and never print anything.

If instead A were registered *first* (before B and C), even `notify(SC_ZERO_TIME)` could fail — A would fire its delta notification before B and C have had their initialization turn, and the event would arrive in delta 1 when neither B nor C is yet waiting.

This is the **missing event problem**: notification ordering relative to thread initialization can cause events to be silently dropped, with no error from the kernel.

## `notify()` Forms — Recap

| Form | Fires in | Time advances? | Safe for init signaling? |
|---|---|---|---|
| `notify()` | Current delta | No | ⚠️ Depends on order |
| `notify(SC_ZERO_TIME)` | Next delta | No | ✅ If sender runs last |
| `notify(t, unit)` | After time `t` | Yes | ✅ Always safe |

`SC_ZERO_TIME` is a special `sc_time` constant equal to zero duration. It forces the notification into the **next** delta cycle rather than the current one, giving all threads that run in the current delta a chance to reach their `wait()` calls first.

## Expected Output

```
A scheduled a_event (delta)!
B got a_event!
C got a_event!
```

All three lines print and the simulation finishes cleanly at `20 s` (though all activity completes in the first two delta cycles at `0 s`).

## `sc_event` Has No Memory

This bears repeating as a design rule:

> If `notify()` fires and no process is waiting, **the notification is lost**. There is no queue, no flag, no way to detect it happened.

This is fundamentally different from OS semaphores or condition variables, which often have state that persists until consumed. If your design requires a process to reliably receive an event regardless of when it starts waiting, consider using `sc_signal` (which holds its last written value) or a shared flag variable checked on wakeup.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `notify(SC_ZERO_TIME)` | Defers notification by one delta cycle — fires at the same simulation time but after current threads yield |
| Missing event | A notification that fires before any process is waiting — silently lost |
| `sc_event` has no memory | Unlike semaphores, an unheard notification is gone forever |
| One-shot thread | A thread with no infinite loop — runs once and exits |
| Multiple waiters | All processes waiting on the same event are woken when it fires |
| Delta cycle ordering | Thread registration order determines initialization execution order |

## Relation to Previous Examples

Example `08_01` showed immediate `notify()` causing time starvation via an infinite A↔B loop. This example shows the opposite failure mode: a one-shot `notify()` that fires too early and is missed entirely. Together, `08_01` and `08_02` cover the two most common `sc_event` misuse patterns:

| Example | Problem | Cause |
|---|---|---|
| `08_01` | Simulation time freezes | Immediate notify in an infinite loop |
| `08_02` | Process blocks forever | Notification fires before waiter is ready |

## What Comes Next

With the pitfalls of raw `sc_event` well understood, the next topic is **`sc_signal`** — a higher-level primitive that carries typed data, retains its last value, and triggers sensitivity-based processes automatically, avoiding many of these ordering issues.