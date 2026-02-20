# SystemC Initialization

This example examines the **initialization phase** of the SystemC simulation — the step that runs all processes once before any simulation time advances — and demonstrates three different strategies a thread can use to control whether and how it participates in that phase.

## Overview

When `sc_start()` is called, the kernel does not immediately jump to simulating events. It first runs an initialization phase in which every registered process (except clocked threads and those explicitly excluded) executes up to its first `wait()`. This is often desirable, but sometimes a process should only run in response to a real event — not at time zero. This example shows the tools available to handle both cases.

## The Initialization Phase — Step by Step

The kernel performs the following in order when `sc_start()` is called:

1. **Update phase** — applies any pending channel updates (e.g. initial signal values set during elaboration)
2. **Add all `SC_METHOD` and `SC_THREAD` processes to the runnable set**, excluding:
   - Processes for which `dont_initialize()` was called
   - Clocked thread processes (`SC_CTHREAD`)
3. **Delta notification phase** — processes all pending notifications
4. **Evaluation phase** — begins the main simulation loop

> In SystemC 2.0 (the current standard), both `SC_METHOD` and `SC_THREAD` processes run during initialization. In SystemC 1.0, only `SC_METHOD` processes did. The three catcher strategies in this example each handle the difference in a different way.

## The Module

```
trigger    — schedules e at 1 s, 3 s, 5 s, ...
catcher_1  — runs at init AND on every event  (SystemC 2.0 behavior)
catcher_2  — skips init via an extra wait(e)  (mimics SystemC 1.0)
catcher_3  — skips init via dont_initialize()  (declarative approach)
```

### `trigger`

```cpp
void trigger() {
    while (true) {
        e.notify(1, SC_SEC);   // schedule e for 1 second from now
        wait(2, SC_SEC);       // then wait 2 seconds before looping
    }
}
```

`e` fires at **1 s, 3 s, 5 s, ...** — odd seconds, every 2 seconds.

---

### `catcher_1` — Runs during initialization (SystemC 2.0 default)

```cpp
void catcher_1() {
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_1 triggered" << std::endl;
        wait(e);   // dynamic sensitivity
    }
}
```

The print statement is at the **top** of the loop, before `wait(e)`. During initialization the kernel runs this thread from the start, so it prints at **0 s** before suspending. It then wakes again at every subsequent event.

---

### `catcher_2` — Skips initialization (mimics SystemC 1.0)

```cpp
void catcher_2() {
    wait(e);   // consume the initialization run — no print
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_2 triggered" << std::endl;
        wait(e);
    }
}
```

A `wait(e)` is placed **before** the loop as a guard. During initialization the thread runs, hits this `wait(e)` immediately, and suspends — printing nothing. The first real print happens when `e` fires at **1 s**. This is the portable, explicit way to mimic SystemC 1.0 thread behavior in a 2.0 simulator.

---

### `catcher_3` — Skips initialization via `dont_initialize()`

```cpp
// In constructor:
SC_THREAD(catcher_3);
sensitive << e;
dont_initialize();
```

```cpp
void catcher_3() {
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_3 triggered" << std::endl;
        wait(e);
    }
}
```

`dont_initialize()` instructs the kernel to **exclude this process from the initialization runnable set**. The process only wakes when a real event from its static sensitivity list occurs. The first print is at **1 s** — identical outcome to `catcher_2`, but declared rather than coded into the thread body.

Note that `dont_initialize()` always applies to the **last registered process** at the time it is called.

---

## `dont_initialize()` vs. Extra `wait()` Guard

Both `catcher_2` and `catcher_3` skip initialization, but via different mechanisms:

| | `catcher_2` (extra `wait`) | `catcher_3` (`dont_initialize`) |
|---|---|---|
| **How** | Thread runs, immediately suspends | Thread never runs during init |
| **Requires static sensitivity?** | No — uses dynamic `wait(e)` | Yes — relies on `sensitive <<` |
| **Portable to SystemC 1.0?** | Yes | Yes |
| **Code change needed?** | Yes — extra `wait()` in body | No — declaration only |
| **Cleaner intent?** | Implicit | ✅ Explicit |

## Execution Timeline

```
Time    trigger         catcher_1       catcher_2       catcher_3
────────────────────────────────────────────────────────────────
0 s     schedules e@1s  prints "0 s"    (guards, silent) (excluded)
1 s     schedules e@3s  prints "1 s"    prints "1 s"    prints "1 s"
3 s     schedules e@5s  prints "3 s"    prints "3 s"    prints "3 s"
(stop @ 4 s)
```

## Expected Output

```
0 s: catcher_1 triggered
1 s: catcher_1 triggered
1 s: catcher_2 triggered
1 s: catcher_3 triggered
3 s: catcher_1 triggered
3 s: catcher_2 triggered
3 s: catcher_3 triggered
```

`catcher_1` is the only one that prints at `0 s`. From `1 s` onwards all three behave identically.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| Initialization phase | Kernel runs all processes once before simulation time advances |
| SystemC 2.0 behavior | Both `SC_METHOD` and `SC_THREAD` run during initialization |
| `dont_initialize()` | Excludes the last registered process from the initialization phase |
| Extra `wait()` guard | Placing `wait(e)` before the loop consumes the initialization run silently |
| Initialization order | Processes run in **unspecified order** during initialization — do not rely on it |
| `sensitive <<` with `dont_initialize()` | Required combination — `dont_initialize()` uses the static sensitivity to know what to wait for |

## Relation to Previous Examples

The elaboration callbacks example (`04_SystemC_Elaboration`) showed the phases *before* `sc_start()`. This example picks up exactly where that left off — examining the first thing that happens *inside* `sc_start()`. The sensitivity example (`09_Sensitivity`) introduced `sensitive <<` and `dont_initialize()` briefly; this example makes `dont_initialize()` the central subject and shows all three initialization strategies side by side.

## What Comes Next

With process lifecycle and initialization fully understood, the next topic is **`sc_signal` and ports** — the standard typed communication channel that brings together signals, events, sensitivity, and delta cycles into a single coherent abstraction for inter-module data flow.