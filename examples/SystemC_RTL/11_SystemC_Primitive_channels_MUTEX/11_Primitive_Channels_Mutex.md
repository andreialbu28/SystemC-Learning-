# SystemC Mutex — `sc_mutex`

This example introduces **`sc_mutex`** — SystemC's built-in primitive channel for mutual exclusion — and demonstrates how multiple concurrent threads safely share a single resource by acquiring and releasing a lock.

## Overview

Previous examples communicated between processes using raw events and shared module variables. As the delta-cycle example (`08_Delta_Cycle`) showed, sharing plain C++ variables between concurrent processes is dangerous because execution order within a timestep is non-deterministic. SystemC provides **primitive channels** as a safer, higher-level alternative. `sc_mutex` is the simplest of these: it ensures that only one process can hold a resource at a time, with all others blocked until it becomes available.

## What Is a Primitive Channel?

SystemC has two types of channels: primitive and hierarchical. Primitive channels are:
- Lightweight — no hierarchy, no simulation processes inside
- Fast — designed for tight simulation loops
- All derived from `sc_prim_channel`

Built-in primitive channels include `sc_mutex`, `sc_semaphore`, `sc_fifo`, and `sc_signal`. This example focuses on `sc_mutex`.

## `sc_mutex` — Behavior and Member Functions

A mutex is always in one of two states: **locked** or **unlocked**. Only one process can hold the lock at a time, and only the process that locked it can unlock it.

### `lock()` — blocking acquire

```cpp
m.lock();
```

- If the mutex is **unlocked**: locks it and returns `0` immediately.
- If the mutex is **locked**: **suspends** the calling process until it becomes available, then locks it and returns `0`.
- If multiple processes try to lock simultaneously in the same delta, the winner is **non-deterministic**.
- Always returns `0`.

### `trylock()` — non-blocking attempt

```cpp
int result = m.trylock();
```

- If the mutex is **unlocked**: locks it and returns `0`.
- If the mutex is **locked**: returns `-1` immediately — the process is **not suspended**.
- Useful for polling or fallback logic.

### `unlock()` — release

```cpp
m.unlock();
```

- If called by the process that locked it: unlocks the mutex, uses **immediate notification** to wake any waiting processes, and returns `0`.
- If called by a different process, or if already unlocked: returns `-1`, mutex state unchanged.

## The Module

Three threads compete for one mutex `m`:

```cpp
SC_THREAD(thread_1);   // occupies mutex for 1 s per cycle, loops forever
SC_THREAD(thread_2);   // occupies mutex for 3 s per cycle, loops forever
SC_THREAD(thread_3);   // occupies mutex for 5 s, runs once then exits
```

Each thread follows the same pattern:

```cpp
if (m.trylock() == -1) {     // attempt non-blocking lock
    m.lock();                // if failed, block until available
}
// ... use resource ...
wait(N, SC_SEC);             // hold mutex for N seconds
m.unlock();                  // release
wait(SC_ZERO_TIME);          // yield so next waiter can acquire
```

The `wait(SC_ZERO_TIME)` after `unlock()` is essential. Because `unlock()` uses immediate notification, any process waiting on `lock()` moves to the runnable set immediately — but won't actually run until the current process yields. The delta wait gives them that opportunity before the unlocking thread loops back and tries to re-acquire.

## Lock Strategy: `trylock()` + `lock()`

The pattern used here is a two-step acquire:

```cpp
if (m.trylock() == -1) {
    // mutex was busy — now block until it's free
    m.lock();
} else {
    // mutex was free — already locked by trylock()
}
```

This lets the thread detect and report contention before committing to a blocking wait. In production code `trylock()` is often used alone (without `lock()`) when a process should skip the resource rather than wait for it.

## Execution Timeline

At time 0, all three threads run during initialization. Registration order is thread_1, thread_2, thread_3:

```
0 s     thread_1 tries trylock → succeeds (mutex was free)
        thread_2 tries trylock → fails (thread_1 holds it), calls lock() → suspends
        thread_3 tries trylock → fails, calls lock() → suspends

1 s     thread_1 unlocks → thread_2 or thread_3 wakes (non-deterministic)
        winner holds mutex

...
```

Because the winner after each unlock is non-deterministic, the exact sequence beyond `0 s` may vary between simulator runs.

## Expected Output (indicative)

```
 - THREAD_1 -->  Trying to lock the mutex and succeeded, returns 0
0 s: thread_1 obtained resource by trylock()
 -- THREAD_2 --> Trying to lock the mutex but fails, returns -1
 --- THREAD_3 --> Trying to lock the mutex but fails, returns -1
1 s: unlocked by thread_1
1 s: thread_2 obtained resource by lock()      ← or thread_3, non-deterministic
4 s: unlocked by thread_2
...
```

Thread_1 always wins the first lock (it runs first during initialization). All subsequent acquisitions after the first unlock are non-deterministic between thread_2 and thread_3.

## `lock()` vs `trylock()` — When to Use Each

| | `lock()` | `trylock()` |
|---|---|---|
| **Blocks if busy?** | Yes — suspends until free | No — returns `-1` immediately |
| **Return value** | Always `0` | `0` (success) or `-1` (busy) |
| **Use when** | Resource is required — must wait | Resource is optional — can skip or retry |
| **Risk** | Can deadlock if unlock is never called | None — never suspends |

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_mutex` | Primitive channel ensuring exclusive resource access |
| `lock()` | Blocking acquire — suspends process until mutex is free |
| `trylock()` | Non-blocking attempt — returns `-1` if already locked |
| `unlock()` | Releases the lock; uses immediate notification to wake waiters |
| `wait(SC_ZERO_TIME)` after unlock | Yields so waiting processes can acquire before the unlocker re-enters |
| Non-deterministic winner | When multiple processes compete for the same lock, the winner is unspecified |
| Primitive channel | Lightweight SystemC channel with no internal processes, derived from `sc_prim_channel` |

## Relation to Previous Examples

The delta cycle example (`08_Delta_Cycle`) exposed the danger of sharing plain C++ variables between concurrent processes. `sc_mutex` is the first primitive channel solution to that problem — it doesn't share data itself, but it **serializes access** to whatever shared resource the processes need. The `wait(SC_ZERO_TIME)` pattern here also reinforces the delta notification lesson from `08_02`.

## What Comes Next

`sc_mutex` controls *access* but carries no data. The next primitive channels — **`sc_semaphore`** and **`sc_fifo`** — add counting and data-passing capabilities, building toward `sc_signal`, the channel used for typed data communication between module ports.