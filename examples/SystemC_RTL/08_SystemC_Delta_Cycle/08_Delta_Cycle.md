# SystemC Delta Cycle

This example demonstrates the **delta cycle** — SystemC's mechanism for ordering process execution within a single simulation timestep — and shows how shared mutable state produces non-deterministic results when multiple threads access it without synchronization.

## Overview

When multiple processes are runnable at the same simulation time, the kernel executes them sequentially within a **delta cycle**: a scheduling step that advances the event queue but does not advance the simulation clock. The order in which those processes run depends on their registration order, and because they share state, the result of the computation changes depending on who runs first.

## The Module

Four threads share two integer variables `x` and `y`, both initialized to `1`:

```cpp
int x = 1, y = 1;

SC_THREAD(add_x);       // x += 2
SC_THREAD(multiply_x);  // x *= 2  (code says *3 but implements *2)
SC_THREAD(add_y);       // y += 2
SC_THREAD(multiply_y);  // y *= 2  (code says *3 but implements *2)
```

Each thread reads the shared variable, prints what it is about to do, modifies it, then prints the result — and exits. No `wait()` calls, no loops.

> **Note:** The `cout` labels say `* 3` but the implementation uses `*= 2`. The printed label is misleading — the actual operation is multiplication by 2.

## What the Delta Cycle Determines

All four threads become runnable at time zero during initialization. The kernel runs them in **registration order**:

```
1. add_x       → reads x=1,  writes x=3
2. multiply_x  → reads x=3,  writes x=6
3. add_y       → reads y=1,  writes y=3
4. multiply_y  → reads y=3,  writes y=6
```

All of this happens within **delta 0 at 0 s** — simulation time never advances.

## Expected Output

```
ADD X: 1 + 2 = 3
MULTIPLY X: 3 * 3 = 6
ADD Y: 1 + 2 = 3
MULTIPLY Y: 3 * 3 = 6
```

The result `x = 6` is registration-order dependent. If `multiply_x` had been registered before `add_x`, the result would be:

```
MULTIPLY X: 1 * 2 = 2     ← reads x before add
ADD X: 2 + 2 = 4          ← reads x after multiply
```

Yielding `x = 4` instead of `x = 6`. Same operations, different order, different answer.

## Why This Is a Problem

In real hardware, `add` and `multiply` would happen on separate computational units operating **truly in parallel** — each reading the original value of `x` simultaneously and producing independent results. Neither would see the other's write.

In SystemC's cooperative model, "concurrent" threads are actually sequential within a delta. Shared plain C++ variables (`int`, `bool`, etc.) have no protection against this ordering sensitivity. The result depends entirely on registration order, which is an implementation detail — not a design intent.

This is the core motivation for `sc_signal`: it separates **read** (current value) from **write** (next value) across a delta boundary, so all processes in a delta see the value from the *previous* delta regardless of execution order.

## Delta Cycle — Conceptual Model

```
Simulation time: 0 s
│
├─ Delta 0
│   ├─ add_x runs       (x: 1 → 3)
│   ├─ multiply_x runs  (x: 3 → 6)
│   ├─ add_y runs       (y: 1 → 3)
│   └─ multiply_y runs  (y: 3 → 6)
│
└─ No more runnable processes → sc_start() returns
```

No time passes. Everything happens in one delta at `0 s`.

## Delta Cycle vs. Simulation Time

| | Delta cycle | Simulation time |
|---|---|---|
| **Triggered by** | Process becoming runnable | `wait(t)` or `notify(t)` |
| **Clock advances?** | No | Yes |
| **Purpose** | Order concurrent processes | Model physical time |
| **Visible in `sc_time_stamp()`?** | No | Yes |

Multiple deltas can occur at the same simulation timestamp. The timestamp only advances when a time-delayed event or `wait(t, unit)` causes the kernel to jump forward.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| Delta cycle | A scheduling step within a single simulation time; clock does not advance |
| Registration order | Determines execution order of threads within the same delta |
| Shared mutable state | Plain C++ variables accessed by multiple threads — order-sensitive and non-deterministic in intent |
| Non-determinism | Same code, different registration order → different results |
| `sc_signal` motivation | Provides delta-separated read/write semantics to eliminate ordering sensitivity |

## Relation to Previous Examples

The concurrency example (`06_SystemC_Concurrency`) showed threads at different timestamps interleaving safely. The events examples showed threads synchronizing deliberately. This example reveals what happens at the lowest level — within a single timestamp — when threads share data without any synchronization. It is the direct motivation for the `sc_signal` abstraction covered next.

## What Comes Next

`sc_signal` solves the problem shown here by buffering writes and applying them only at the next delta boundary — ensuring all readers in a delta always see a consistent, order-independent value. The next example introduces `sc_signal` and ports as the proper way to share data between concurrent SystemC processes.