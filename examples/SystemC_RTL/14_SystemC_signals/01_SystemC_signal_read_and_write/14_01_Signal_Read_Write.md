# SystemC Signals — Read and Write (`sc_signal`)

This example introduces **`sc_signal<T>`** — the cornerstone primitive channel for typed, hardware-style value propagation in SystemC — and demonstrates its most important property: writes do not take effect until the next delta cycle.

## Overview

`sc_signal` is the channel that underlies nearly all structural SystemC design. Unlike a plain C++ variable (where a write is instantly visible) or `sc_fifo` (a queue), `sc_signal` models a hardware net: it holds a single current value, a pending next value, and notifies sensitive processes when the value changes — but only after the current delta completes. This delta-separation is what makes `sc_signal` safe to share between concurrent processes regardless of execution order.

This sub-example focuses purely on the read/write API and the delta-cycle update behaviour before introducing ports and inter-module connections in later sub-examples.

## `sc_signal<T>` — Core Behaviour

### Writing

```cpp
s.write(v);   // explicit write method
s = v;        // assignment operator — equivalent to write()
```

A write does **not** update the signal immediately. It schedules an update for the end of the current delta cycle. Until the delta completes, `s.read()` still returns the previous value.

### Reading

```cpp
s.read()   // explicit read method — returns current value
int v = s; // implicit conversion — equivalent to read()
s          // used directly in cout — also calls read()
```

Reading always returns the **current** (committed) value — the value from the last completed delta, not the pending write.

### Initial value

```cpp
signal.s = -1;   // set before sc_start() — takes effect at elaboration
```

An assignment to an `sc_signal` during elaboration (before `sc_start()`) sets its initial value. This is applied during the update phase at the very start of simulation.

## The Example Walkthrough

```cpp
void readwrite() {
    s.write(3);
    std::cout << "s = " << s << "; s.read() = " << s.read() << std::endl;
    wait(SC_ZERO_TIME);
    std::cout << "after delta_cycle, s = " << s << std::endl;

    s = 4;
    s = 5;
    int tmp = s;
    std::cout << "s = " << tmp << std::endl;
    wait(SC_ZERO_TIME);
    std::cout << "after delta_cycle, s = " << s.read() << std::endl;
}
```

### Step 1 — Write 3, read before delta

```cpp
s.write(3);
std::cout << "s = " << s << "; s.read() = " << s.read() << std::endl;
```

`s` was initialised to `-1` during elaboration. `write(3)` schedules `3` as the next value but does not commit it yet. Both `s` (implicit read) and `s.read()` still return **-1**.

```
s = -1; s.read() = -1
```

### Step 2 — Wait one delta, read after commit

```cpp
wait(SC_ZERO_TIME);
std::cout << "after delta_cycle, s = " << s << std::endl;
```

`wait(SC_ZERO_TIME)` yields to the kernel, which runs the update phase and commits the pending write. On resumption `s` now holds **3**.

```
after delta_cycle, s = 3
```

### Step 3 — Multiple writes before delta

```cpp
s = 4;
s = 5;
int tmp = s;
std::cout << "s = " << tmp << std::endl;
```

Two writes are issued in the same delta. Only the **last write wins** — the intermediate value `4` is discarded. `s` still reads **3** (the previously committed value) because no delta has passed yet. `tmp` captures the current value `3`.

```
s = 3
```

### Step 4 — After delta, last write committed

```cpp
wait(SC_ZERO_TIME);
std::cout << "after delta_cycle, s = " << s.read() << std::endl;
```

After the delta, `5` is committed (the last write). `4` was overwritten before the update phase ran and is permanently lost.

```
after delta_cycle, s = 5
```

## Full Expected Output

```
s = -1; s.read() = -1
after delta_cycle, s = 3
s = 3
after delta_cycle, s = 5
```

## Delta-Deferred Update — Why It Matters

This behaviour directly solves the problem demonstrated in `08_Delta_Cycle`: when multiple concurrent processes all write to the same signal in the same delta, **every process reads the same pre-delta value** regardless of who runs first. The update is applied atomically at the end of the delta, after all processes have had their turn. Execution order no longer affects the result.

```
Delta N:  process A writes s=10, reads s (gets old value)
          process B writes s=20, reads s (gets old value)
          ── update phase ──  last write (20) committed
Delta N+1: both A and B read s = 20
```

## `sc_signal<T>` vs. Plain Variable vs. `sc_fifo`

| | Plain `int` | `sc_fifo<int>` | `sc_signal<int>` |
|---|---|---|---|
| **Holds** | One value | Queue of values | One value |
| **Write visible** | Immediately | After read | Next delta |
| **Multiple writers** | Race condition | Non-deterministic | Last write wins |
| **Notifies processes?** | No | Via events | Yes, on value change |
| **Models** | Software variable | Streaming data | Hardware net / wire |

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_signal<T>` | Primitive channel holding a single typed value with delta-deferred updates |
| `write(v)` / `s = v` | Schedules a pending update — does not commit immediately |
| `read()` / `int v = s` | Returns the current committed value |
| Delta-deferred update | Writes are committed at the end of the current delta cycle |
| Last write wins | Multiple writes in one delta — only the final value is committed |
| Elaboration assignment | `signal.s = -1` before `sc_start()` sets the initial value |
| `wait(SC_ZERO_TIME)` | Yields to let the kernel run the update phase and commit pending writes |

## Relation to Previous Examples

The delta cycle example (`08_Delta_Cycle`) showed that sharing plain C++ variables between concurrent threads produces order-dependent results. `sc_signal` is the solution: its delta-deferred write model ensures all readers in a given delta see the same consistent value. This example isolates the read/write mechanics before combining `sc_signal` with ports and sensitivity in subsequent sub-examples.

## What Comes Next

The next sub-example (`02_SystemC_signal_event`) explores the event and sensitivity side of `sc_signal` — specifically the `value_changed_event()`, `posedge_event()`, and `negedge_event()` notifications that allow processes to react automatically whenever a signal changes value.