# SystemC Semaphore — `sc_semaphore`

This example introduces **`sc_semaphore`** — SystemC's primitive channel for managing a pool of identical resources — and shows how three competing threads share a resource pool of size 2, with the semaphore value acting as a live count of remaining availability.

## Overview

A mutex (`sc_mutex`) is an all-or-nothing lock: exactly one process holds it at a time. A semaphore generalises this to **N concurrent holders**. It is the right abstraction when you have multiple identical resources — parking spaces, network connections, DMA channels — and you want any N processes to proceed simultaneously while the rest wait.

In SystemC, a mutex is simply a semaphore initialised to 1.

## `sc_semaphore` — Behaviour and Member Functions

A semaphore holds an integer **value** set at construction, representing the number of available resources. Acquiring decrements it; releasing increments it.

```cpp
sc_semaphore s(N);   // N resources available
```

### `wait()` — blocking acquire

```cpp
s.wait();
```

- If value **> 0**: decrements value and returns `0` immediately.
- If value **== 0**: **suspends** the calling process until another process calls `post()`.
- Always returns `0`.

### `trywait()` — non-blocking attempt

```cpp
int result = s.trywait();
```

- If value **> 0**: decrements value and returns `0`.
- If value **== 0**: returns `-1` immediately — process is **not suspended**.

### `post()` — release

```cpp
s.post();
```

- Increments the semaphore value.
- Uses **immediate notification** to wake any process suspended in `wait()`.
- Always returns `0`.

### `get_value()` — inspect

```cpp
int available = s.get_value();
```

Returns the current semaphore value — the number of resources still available. Useful for logging and diagnostics.

## The Module

```cpp
sc_semaphore s;

SC_CTOR(SEMAPHORE) : s(2)   // 2 resources available
{
    SC_THREAD(thread_1);    // holds resource for 2 s
    SC_THREAD(thread_2);    // holds resource for 1 s
    SC_THREAD(thread_3);    // holds resource for 1 s
}
```

The semaphore is initialised with `2` in the constructor initializer list. All three threads compete for these two slots using the same two-step pattern:

```cpp
if (s.trywait() == -1) {   // non-blocking attempt
    s.wait();              // blocked wait if resource unavailable
}
// ... use resource for N seconds ...
s.post();                  // release
wait(SC_ZERO_TIME);        // yield so waiters can acquire
```

## Initialization at Time Zero

All three threads become runnable during initialization and execute in registration order (thread_1, thread_2, thread_3). The semaphore starts at `2`:

```
thread_1 → trywait() succeeds → value: 2→1
thread_2 → trywait() succeeds → value: 1→0
thread_3 → trywait() fails → calls wait() → suspends
```

Two threads hold resources; the third blocks immediately. The semaphore value reaches `0` and stays there until a `post()` occurs.

## Execution Timeline

```
Time    thread_1 (holds 2s)   thread_2 (holds 1s)   thread_3 (holds 1s)   value
──────────────────────────────────────────────────────────────────────────────────
0 s     acquires              acquires              BLOCKED               0
1 s     (holding)             posts → value=1       wakes, acquires→0
2 s     posts → value=1       acquires→0            (holding)
3 s     acquires→0            posts → value=1       posts → value=1 (→2 briefly)
...
```

Because `post()` uses immediate notification and thread_3 is already waiting, it wakes in the same delta as the `post()` — so the value may briefly tick up and back down within a single delta.

## Expected Output (first few lines)

```
0 s: locked by thread_1, value is 1
0 s: locked by thread_2, value is 0
0 s: THREAD 3: resource not available, value of semaphore is 0
1 s: unlocked by thread_2, value is 1
1 s: locked by thread_3, value is 0
2 s: unlocked by thread_1, value is 1
2 s: locked by thread_2, value is 0      ← or thread_3 post — non-deterministic
...
```

After the first unlock the acquisition order between competing threads is non-deterministic, as with `sc_mutex`.

## `sc_semaphore` vs. `sc_mutex`

| | `sc_mutex` | `sc_semaphore` |
|---|---|---|
| **Capacity** | 1 | N (set at construction) |
| **Acquire** | `lock()` / `trylock()` | `wait()` / `trywait()` |
| **Release** | `unlock()` | `post()` |
| **Ownership** | Only locker can unlock | Any process can `post()` |
| **Inspect value** | No | `get_value()` |
| **Use when** | Single exclusive resource | Pool of identical resources |

The key ownership difference: `sc_mutex` enforces that only the locking process can unlock. `sc_semaphore` has no such restriction — any process can call `post()`, making it suitable for producer/consumer patterns where the releaser is not the acquirer.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_semaphore s(N)` | Creates a semaphore with N available resources |
| `wait()` | Blocking acquire — suspends if value is 0 |
| `trywait()` | Non-blocking acquire — returns `-1` if value is 0 |
| `post()` | Releases one resource; immediate notification wakes waiters |
| `get_value()` | Returns current number of available resources |
| Semaphore value | Live counter of remaining available slots |
| No ownership rule | Any process may call `post()`, unlike `sc_mutex::unlock()` |

## Relation to Previous Examples

`sc_mutex` (`11_Mutex`) is the special case of a semaphore with value 1. This example generalises that to `N=2`, making two simultaneous holders possible while still blocking a third. The `trywait()` + `wait()` pattern mirrors the `trylock()` + `lock()` pattern from the mutex example, and the `wait(SC_ZERO_TIME)` after release serves the same purpose — yielding so waiting processes can acquire before the releaser loops back.

## What Comes Next

`sc_mutex` and `sc_semaphore` manage *access* but carry no data. The next primitive channel — **`sc_fifo`** — combines flow control with data transfer, allowing processes to pass typed values through a bounded buffer.