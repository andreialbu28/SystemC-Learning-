# SystemC Constructor & Process Types

This example introduces the three types of SystemC processes — `SC_METHOD`, `SC_THREAD`, and `SC_CTHREAD` — and shows how they are registered inside the module constructor using `SC_CTOR` with an initializer list.

## Overview

Building on the previous example (which showed how to *declare* a module), this example focuses on what goes *inside* the constructor: registering processes and initializing member objects like clocks. It also demonstrates how simulation time works by running the kernel for a fixed duration and observing when each process type fires.

## The Module

```cpp
SC_MODULE(PROCESS) {
    sc_clock clk;

    SC_CTOR(PROCESS) : clk("clk", 1, SC_SEC) {
        SC_METHOD(method);
        SC_THREAD(thread);
        SC_CTHREAD(cthread, clk);
    }

    void method(void);
    void thread(void);
    void cthread(void);
};
```

### Constructor Initializer List

`SC_CTOR` expands to a constructor that only accepts an `sc_module_name`. To initialize member objects that themselves require constructor arguments (like `sc_clock`), you use a **C++ initializer list**:

```cpp
SC_CTOR(PROCESS) : clk("clk", 1, SC_SEC)
```

Here `clk` is initialized with a name, a period of `1`, and a time unit of `SC_SEC` (one second). The initializer list runs before the constructor body, so `clk` is fully constructed by the time the process registrations execute inside the braces.

## The Three Process Types

### `SC_METHOD` — Lightweight, no internal state

```cpp
SC_METHOD(method);
```

```cpp
void method(void) {
    std::cout << "Method triggered @ " << sc_time_stamp() << std::endl;
    next_trigger(sc_time(1, SC_SEC));
}
```

An `SC_METHOD` is the lightest process type. It runs to completion every time it is triggered and **cannot suspend itself** — there is no `wait()` inside an `SC_METHOD`. To schedule the next execution, `next_trigger()` is called to tell the kernel when to invoke it again. Because it has no loop and no private stack, it is the most resource-efficient process type and maps naturally to **combinational logic**.

---

### `SC_THREAD` — Suspendable, free-running

```cpp
SC_THREAD(thread);
```

```cpp
void thread(void) {
    while (true) {
        std::cout << "Thread triggered @ " << sc_time_stamp() << std::endl;
        wait(500, SC_MS);
    }
}
```

An `SC_THREAD` is spawned once at the start of simulation and is expected to contain an infinite loop. It can suspend itself at any point using `wait()`, yielding control back to the kernel until the specified time has elapsed. Unlike `SC_METHOD`, it maintains its own execution stack and resumes exactly where it left off after each `wait()`. This makes it well suited for **behavioral and transaction-level modeling**.

Here the thread wakes every **500 ms**, making it twice as frequent as the clock.

---

### `SC_CTHREAD` — Clock-synchronous thread

```cpp
SC_CTHREAD(cthread, clk);
```

```cpp
void cthread(void) {
    while (true) {
        std::cout << "Cthread triggered @ " << sc_time_stamp() << std::endl;
        wait();
    }
}
```

An `SC_CTHREAD` is like an `SC_THREAD` but explicitly tied to a clock signal. Its `wait()` (called with no arguments) always waits for the **next active edge of the associated clock** rather than a time duration. This makes it the natural choice for modeling **synchronous, clocked hardware** such as registers, pipelines, and state machines.

Here `cthread` is bound to `clk`, which has a 1-second period, so it fires once per second aligned to the clock edge.

---

## Process Comparison

| | `SC_METHOD` | `SC_THREAD` | `SC_CTHREAD` |
|---|---|---|---|
| **Has infinite loop?** | No | Yes | Yes |
| **Can call `wait()`?** | No | Yes | Yes (no-arg only) |
| **Rescheduling** | `next_trigger()` | `wait(time)` | `wait()` on clock edge |
| **Clock sensitivity** | No | No | Yes (required) |
| **Stack overhead** | None | Yes | Yes |
| **Typical use** | Combinational logic | Behavioral/TLM modeling | Synchronous/RTL logic |

## Simulation Run

```cpp
sc_start(5, SC_SEC);
```

The kernel runs for exactly 5 seconds of simulation time. During this window:

- `method` fires at **0 s, 1 s, 2 s, 3 s, 4 s** — every 1 second via `next_trigger`
- `thread` fires at **0 s, 0.5 s, 1 s, 1.5 s, ...** — every 500 ms via `wait`
- `cthread` fires at **0 s, 1 s, 2 s, 3 s, 4 s** — every clock edge via `wait()`

### Expected Output

```
Execution phase begins @ 0 s
Method triggered @ 0 s
Thread triggered @ 0 s
Cthread triggered @ 0 s
Thread triggered @ 500000000 ps
Method triggered @ 1 s
Thread triggered @ 1 s
Cthread triggered @ 1 s
Thread triggered @ 1500000000 ps
...
Execution time ends @ 5 s
```

> SystemC internally represents time in picoseconds, so 500 ms may display as `500000000 ps` depending on the configured time resolution.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_clock` | A built-in SystemC signal that toggles at a fixed period |
| Initializer list with `SC_CTOR` | How to initialize member objects that need their own constructor arguments |
| `sc_time_stamp()` | Returns the current simulation time |
| `next_trigger()` | Reschedules an `SC_METHOD` for a future time |
| `wait(time)` | Suspends an `SC_THREAD` for a given duration |
| `wait()` | Suspends an `SC_CTHREAD` until the next clock edge |
| `sc_start(time)` | Runs the simulation kernel for a bounded duration |

## What Comes Next

With the three process types understood, the next natural topics are **ports and signals** — connecting multiple modules together so that events in one module can trigger processes in another.