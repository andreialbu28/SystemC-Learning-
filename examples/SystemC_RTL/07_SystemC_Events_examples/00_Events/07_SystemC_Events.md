# SystemC Events

This example introduces `sc_event` — SystemC's fundamental synchronization primitive — by building a **circular event chain** across three concurrent threads that take turns waking each other up in a fixed order.

## Overview

In the previous concurrency example, threads ran independently on fixed timers. In real hardware designs, processes rarely run on clocks alone — they need to react to each other. `sc_event` is the mechanism SystemC provides for one process to signal another, decoupling the sender from the receiver without requiring shared variables or polling.

## Project Structure

```
├── include/
│   └── ordered_events.h        # Module declaration with sc_event members
├── src/
│   ├── ordered_events.cpp      # Thread implementations
│   └── main.cpp                # sc_main entry point
```

## What Is `sc_event`?

An `sc_event` is a notification object. It carries no data — it is a pure synchronization signal. Any process can call `.notify()` on it, and any process blocked on `wait(event)` will be resumed when that notification arrives.

```cpp
sc_event a_event;        // declaration — no initial state
a_event.notify(1, SC_NS); // schedule a notification 1 ns in the future
wait(a_event);           // suspend until a_event is notified
```

Key properties:
- An `sc_event` has **no memory** — if a notification fires while no process is waiting, it is lost.
- Multiple processes can wait on the same event; all will be woken when it fires.
- `notify()` with a time argument schedules a **deferred notification** — the event fires after the specified delay, not immediately.

## The Circular Chain

Three threads and three events form a closed ring:

```
A ──(a_event)──► B ──(b_event)──► C ──(c_event)──► A
│                                                    │
└────────────────────────────────────────────────────┘
```

Each thread:
1. Notifies its own outgoing event (deferred by 1 ns)
2. Waits on the incoming event from the previous thread in the chain

```cpp
void A_thread() {
    while (true) {
        a_event.notify(1, SC_NS);   // signal B, arriving 1 ns later
        wait(c_event);              // block until C signals back
    }
}

void B_thread() {
    while (true) {
        b_event.notify(1, SC_NS);   // signal C
        wait(a_event);              // block until A signals
    }
}

void C_thread() {
    while (true) {
        c_event.notify(1, SC_NS);   // signal A
        wait(b_event);              // block until B signals
    }
}
```

At time zero, all three threads run their first iteration simultaneously (registration order: B, A, C), each scheduling a notification 1 ns ahead and then immediately blocking. One nanosecond later, all three notifications fire, unblocking all three threads — and the cycle repeats.

## Registration Order vs. Execution Order

Notice the threads are registered in the constructor in the order **B, A, C**:

```cpp
SC_THREAD(B_thread);
SC_THREAD(A_thread);
SC_THREAD(C_thread);
```

This affects the order in which threads run within the same simulation timestep (delta cycle). When multiple threads become runnable at the same time, the kernel runs them in registration order. The output at each nanosecond boundary will therefore show B printing before A, and A before C.

## Expected Output

```
=== Ordered Events Circular Dependency Example ===
Event chain: A → B → C → A
...
0 s B sent b_event
0 s A sent a_event!
0 s C sent c_event
1 ns B got a_event!
1 ns B sent b_event
1 ns A got c_event
1 ns A sent a_event!
1 ns C got b_event
1 ns C sent c_event
2 ns B got a_event!
2 ns B sent b_event
...
Simulation finished at 15 ns
```

Each nanosecond, all three threads complete one full cycle: receive their incoming event, fire their outgoing event, and block again. The pattern repeats cleanly for the full 15 ns simulation window.

## Deferred vs. Immediate Notification

This example uses `notify(1, SC_NS)` — a **deferred notification** that fires after a time delay. SystemC also supports:

| Form | Behavior |
|---|---|
| `event.notify()` | Immediate — fires in the current delta cycle |
| `event.notify(SC_ZERO_TIME)` | Delta-delayed — fires in the next delta cycle at the same time step |
| `event.notify(t, unit)` | Time-delayed — fires after the specified simulation time |

Using a 1 ns delay here ensures simulation time actually advances with each cycle. If `notify()` were called with no arguments (immediate), all three threads could complete their full cycle within a single delta, potentially causing an infinite loop at a single timestamp.

## `sc_event` vs. `sc_clock`

| | `sc_clock` | `sc_event` |
|---|---|---|
| **Periodicity** | Fixed, automatic | Manual, on demand |
| **Data** | None (toggle signal) | None |
| **Direction** | Broadcast | Targeted or broadcast |
| **Typical use** | Clocked processes (`SC_CTHREAD`) | Inter-process signaling |

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_event` | A stateless synchronization primitive for signaling between processes |
| `event.notify(t, unit)` | Schedules a deferred notification after time `t` |
| `wait(event)` | Suspends the calling thread until the specified event is notified |
| Circular dependency | A closed ring of events where each process both produces and consumes |
| Registration order | Determines the execution order of threads within the same delta cycle |
| Deferred vs. immediate notify | Controls whether simulation time advances between notifications |

## Relation to Previous Examples

The concurrency example (`06_SystemC_Concurrency`) showed threads running independently on fixed timers. This example replaces fixed timers with **event-driven synchronization** — threads now only run when explicitly signaled by another thread, giving full control over execution order regardless of real time.

## What Comes Next

With events understood, the next step is **sensitive processes and signals** — using `sc_signal` to carry data between modules and automatically triggering processes when values change.