# SystemC Events — Combined Event Expressions

This example is a comprehensive reference for all the ways a SystemC thread can wait on events — from a simple timeout to complex boolean combinations of multiple events with an optional deadline.

## Overview

Previous examples used `wait(event)` and `wait(time)` individually. SystemC's `wait()` is significantly more expressive than that: events can be combined with `&` (AND) and `|` (OR) operators, and any event expression can be paired with a timeout so the process wakes up whichever comes first. This example exercises every form in a single module.

## The Setup

One `trigger` thread schedules ten events at known times, and seven `catcher` threads each demonstrate a different `wait()` form:

```
e1  fires @ 1s      e6  fires @ 6s
e2  fires @ 2s      e7  fires @ 7s
e3  fires @ 3s      e8  fires @ 8s
e4  fires @ 4s      e9  fires @ 9s
e5  fires @ 5s      e10 fires @ 10s
```

The trigger thread fires all ten notifications at the start of simulation (deferred), then exits. There are no loops — every catcher runs once and exits, so `sc_start()` with no argument terminates naturally when all processes are done.

## The `wait()` Forms

### Form 1 — Pure time timeout

```cpp
void catcher_0() {
    wait(2, SC_SEC);
    // prints @ 2 s
}
```

Suspends for a fixed duration regardless of any events. The simplest form — equivalent to a sleep.

---

### Form 2 — Wait for a single event

```cpp
void catcher_1() {
    wait(e1);
    // prints @ 1 s
}
```

Suspends until `e1` is notified. No timeout — the thread would wait forever if `e1` never fired.

---

### Form 3 — Wait for event A **AND** event B

```cpp
void catcher_2and3() {
    wait(e2 & e3);
    // prints @ 3 s
}
```

The `&` operator creates an AND expression. The thread only wakes when **both** `e2` and `e3` have been notified. Since `e2` fires at 2 s and `e3` at 3 s, the thread wakes at **3 s** — the later of the two.

---

### Form 4 — Wait for event A **OR** event B

```cpp
void catcher_4or5() {
    wait(e4 | e5);
    // prints @ 4 s
}
```

The `|` operator creates an OR expression. The thread wakes when **either** `e4` or `e5` is notified — whichever comes first. Since `e4` fires at 4 s and `e5` at 5 s, the thread wakes at **4 s**.

---

### Form 5 — Timeout **or** single event

```cpp
void catcher_timeout_or_6() {
    wait(sc_time(5, SC_SEC), e6);
    // prints @ 5 s
}
```

Waits for `e6`, but with a 5-second deadline. `e6` is scheduled for 6 s, which is after the 5-second timeout — so the **timeout fires first** at **5 s**. The thread cannot distinguish which condition triggered it from the `wait()` call alone.

---

### Form 6 — Timeout **or** (event A **OR** event B)

```cpp
void catcher_timeout_or_7or8() {
    wait(sc_time(20, SC_SEC), e7 | e8);
    // prints @ 7 s
}
```

Waits for either `e7` or `e8`, with a 20-second deadline. `e7` fires at 7 s, well within the 20-second window — so the **event wins** at **7 s**.

---

### Form 7 — Timeout **or** (event A **AND** event B)

```cpp
void catcher_timeout_or_9and10() {
    wait(sc_time(20, SC_SEC), e9 & e10);
    // prints @ 10 s
}
```

Waits until **both** `e9` and `e10` have fired, or until 20 seconds elapse. `e9` fires at 9 s and `e10` at 10 s — both arrive within the 20-second window, so the thread wakes when the last one arrives at **10 s**.

---

## All `wait()` Forms — Reference Table

| Catcher | `wait()` form | Wakes at | Reason |
|---|---|---|---|
| `catcher_0` | `wait(t, unit)` | **2 s** | Fixed timeout |
| `catcher_1` | `wait(e1)` | **1 s** | e1 fires at 1 s |
| `catcher_2and3` | `wait(e2 & e3)` | **3 s** | Both required; e3 is last |
| `catcher_4or5` | `wait(e4 \| e5)` | **4 s** | Either sufficient; e4 is first |
| `catcher_timeout_or_6` | `wait(sc_time(5,SC_SEC), e6)` | **5 s** | Timeout beats e6 (@ 6 s) |
| `catcher_timeout_or_7or8` | `wait(sc_time(20,SC_SEC), e7\|e8)` | **7 s** | e7 arrives within timeout |
| `catcher_timeout_or_9and10` | `wait(sc_time(20,SC_SEC), e9&e10)` | **10 s** | Both arrive within timeout |

## Expected Output

```
2 s: 2 seconds timeout - function catcher 0
1 s: catch e1 - function catcher 1
3 s: catch e2 and e3 - function catcher 2 and 3
4 s: catch e4 or e5 - function catcher 4 and 5
5 s: 5 seconds timeout or catch e6
7 s: 20 seconds timeout or catch e7 or catch e8
10 s: 20 seconds timeout or catch (e9 and e10)
```

> Note: output ordering in the terminal may differ slightly for threads that wake at the same timestamp, but each line will show the correct wakeup time.

## Automatic Simulation Termination

```cpp
sc_start(); // no time limit
```

No explicit stop time is passed to `sc_start()`. The simulation terminates naturally because every thread runs once and exits — once all processes are done, the kernel has nothing left to schedule and returns on its own. This is a clean alternative to `sc_stop()` for designs where all processes are naturally finite.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `wait(t, unit)` | Pure timeout — wakes after fixed duration |
| `wait(e)` | Single event wait — wakes when `e` is notified |
| `wait(e1 & e2)` | AND — wakes when both events have been notified |
| `wait(e1 \| e2)` | OR — wakes when either event is notified |
| `wait(sc_time(t,u), e)` | Timeout OR event — whichever comes first |
| `wait(sc_time(t,u), e1\|e2)` | Timeout OR (e1 OR e2) |
| `wait(sc_time(t,u), e1&e2)` | Timeout OR (e1 AND e2) |
| Natural termination | `sc_start()` returns when no runnable processes remain |

## Relation to Previous Examples

Examples `08_01` and `08_02` focused on the dangers of `notify()` timing. This example shifts focus to the **receiver side** — the full expressiveness of `wait()` — and serves as a practical reference card for the event-waiting API. It deliberately avoids infinite loops so the focus stays on the `wait()` forms rather than thread lifecycle management.

## What Comes Next

With the full event API covered, the next topic is **`sc_signal`** — a higher-level primitive that carries typed data between modules and automatically triggers sensitive processes when its value changes, building on the event foundation established in this series.