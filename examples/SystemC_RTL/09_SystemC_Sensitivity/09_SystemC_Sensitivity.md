# SystemC Sensitivity

This example introduces the concept of **process sensitivity** — the set of events or timeouts that can wake a process — and demonstrates the two forms SystemC supports: **static sensitivity**, declared once during elaboration, and **dynamic sensitivity**, which a process can change at runtime on each iteration.

## Overview

Every time a SystemC process calls `wait()`, it is suspended until something in its sensitivity set occurs. That "something" can be defined in two places: fixed at elaboration time in the sensitivity list (`sensitive <<`), or specified dynamically inside the process body with `wait(event)`. Understanding the difference — and knowing when each is appropriate — is fundamental to writing correct, predictable SystemC models.

## The Module

Two trigger threads fire events at regular intervals, and two catcher threads receive them — one using dynamic sensitivity, one using static:

```cpp
sc_event event1, event2;

SC_THREAD(trigger_1);          // fires event1 every 2 s
SC_THREAD(trigger_2);          // fires event2 every 3 s
SC_THREAD(catch_1or2_dyn);     // dynamic: wait(event1 | event2)
SC_THREAD(catch_1or2_static);  // static:  wait() — uses sensitivity list
```

The sensitivity list is declared in the constructor:

```cpp
sensitive << event1 << event2;
```

## Static Sensitivity

Static sensitivity is declared **once during elaboration** using the `sensitive` object in the constructor. It applies to the **last process registered** before it — in this case `catch_1or2_static`.

```cpp
SC_THREAD(catch_1or2_static);
sensitive << event1 << event2;   // applies to catch_1or2_static
```

Inside the thread, `wait()` with no arguments means: *suspend and wait for any event in my static sensitivity list*:

```cpp
void catch_1or2_static() {
    while (true) {
        wait();   // wakes on event1 OR event2 — defined by sensitive <<
        std::cout << "Static sensitivity: event1 or event2 @ " << sc_time_stamp() << std::endl;
    }
}
```

The sensitivity is fixed for the lifetime of the process. It cannot be changed at runtime. This makes static sensitivity predictable and easy to reason about — the process always wakes on the same set of triggers.

## Dynamic Sensitivity

Dynamic sensitivity is specified **inside the process body** on each call to `wait()`:

```cpp
void catch_1or2_dyn() {
    while (true) {
        wait(event1 | event2);   // sensitivity decided here, each iteration
        std::cout << "Dynamic sensitivity: event1 or event2 @ " << sc_time_stamp() << std::endl;
    }
}
```

Here the sensitivity is `event1 | event2` on every iteration, so the behavior happens to be identical to the static version. The power of dynamic sensitivity is that it can **change between iterations** — a process could wait on `event1` in one cycle and `event2 & event3` in the next, adapting its behavior based on state.

## Why Triggers Use `wait(SC_ZERO_TIME)` First

```cpp
void trigger_1() {
    wait(SC_ZERO_TIME);  // yield for one delta before firing
    while (true) {
        event1.notify();
        wait(2, SC_SEC);
    }
}
```

This is the missing-event guard from example `08_02`. By waiting one delta cycle before notifying, the trigger ensures that both catcher threads have already reached their `wait()` calls during initialization. Without this delta delay, notifications could fire before the catchers are listening.

## Execution Timeline

`event1` fires every 2 s, `event2` every 3 s. Both catchers wake on either event:

```
Time    Event       Catchers wake
─────────────────────────────────
0 s     e1, e2      dyn ✓  static ✓  (both fire at delta 1)
2 s     e1          dyn ✓  static ✓
3 s     e2          dyn ✓  static ✓
4 s     e1          dyn ✓  static ✓
6 s     e1, e2      dyn ✓  static ✓
```

At 0 s and 6 s both events fire simultaneously (within the same delta). Each catcher wakes once per event notification — so at those times each catcher prints twice.

## Expected Output

```
Dynamic sensitivity:  event1 or event2 @ 0 s
Static sensitivity:   event1 or event2 @ 0 s
Dynamic sensitivity:  event1 or event2 @ 0 s
Static sensitivity:   event1 or event2 @ 0 s
Dynamic sensitivity:  event1 or event2 @ 2 s
Static sensitivity:   event1 or event2 @ 2 s
Dynamic sensitivity:  event1 or event2 @ 3 s
Static sensitivity:   event1 or event2 @ 3 s
Dynamic sensitivity:  event1 or event2 @ 4 s
Static sensitivity:   event1 or event2 @ 4 s
Dynamic sensitivity:  event1 or event2 @ 6 s
Static sensitivity:   event1 or event2 @ 6 s
Dynamic sensitivity:  event1 or event2 @ 6 s
Static sensitivity:   event1 or event2 @ 6 s
```

The `dynamic` and `static` lines always appear together because both threads react to the same events. The interleaving between the two within the same timestamp depends on registration order.

## Static vs. Dynamic Sensitivity — Comparison

| | Static sensitivity | Dynamic sensitivity |
|---|---|---|
| **Declared in** | Constructor (`sensitive <<`) | Process body (`wait(event)`) |
| **When set** | Elaboration — fixed forever | Runtime — can change each iteration |
| **`wait()` call** | `wait()` — no arguments | `wait(event)`, `wait(e1\|e2)`, etc. |
| **Flexibility** | Low — same triggers always | High — can adapt to state |
| **Typical use** | `SC_METHOD`, simple reactive logic | `SC_THREAD` with conditional behavior |
| **Applies to** | Last registered process | The calling process at this `wait()` |

## The `sensitive <<` Chain

The `sensitive` object uses the stream-insertion operator to accumulate events:

```cpp
sensitive << event1 << event2;
```

This is syntactic sugar — each `<<` adds one event to the sensitivity list of the most recently registered process. You can chain as many events as needed. For `sc_clock` signals you would write `sensitive << clk.pos()` to trigger only on rising edges.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| Static sensitivity | Fixed set of triggers declared with `sensitive <<` during elaboration |
| Dynamic sensitivity | Per-iteration triggers specified with `wait(event)` inside the process |
| `sensitive << e` | Adds event `e` to the static sensitivity of the last registered process |
| `wait()` (no args) | Suspends until any event in the static sensitivity list fires |
| `wait(e1 \| e2)` | Suspends until either event fires — dynamic, overrides static for this iteration |
| Sensitivity applies to last process | `sensitive <<` always refers to the most recently registered process |

## Relation to Previous Examples

The events examples (`07`, `08`) used `wait(event)` exclusively — always dynamic. This example introduces the static alternative and places both side by side so the distinction is immediately clear. The `SC_ZERO_TIME` guard reappears here as good practice, reinforcing the lesson from `08_02`.

## What Comes Next

With sensitivity fully covered, the next topic is **`sc_signal` and ports** — the standard mechanism for passing typed data between modules, which builds directly on the event and sensitivity system to automatically trigger sensitive processes whenever a signal's value changes.