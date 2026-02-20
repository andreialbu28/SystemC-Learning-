# SystemC FIFO — `sc_fifo`

This example introduces **`sc_fifo<T>`** — SystemC's built-in primitive channel for bounded, typed data transfer between concurrent processes — and demonstrates all three usage styles: blocking I/O, non-blocking I/O with event-based retry, and capacity inspection.

## Overview

Previous primitive channels (`sc_mutex`, `sc_semaphore`) controlled *access* but carried no data. `sc_fifo` combines flow control with data transfer: it is a bounded queue where a producer writes values and a consumer reads them, with the channel itself handling synchronization. If the FIFO is full, writers wait; if empty, readers wait — automatically.

## The Module

Three independent producer/consumer pairs share three FIFOs, each of capacity 2:

```cpp
sc_fifo<int> f1, f2, f3;

SC_CTOR(FIFO) : f1(2), f2(2), f3(2)
```

Each pair demonstrates a different access style:

| Pair | Style | FIFO |
|---|---|---|
| generator1 / consumer1 | Blocking read/write | `f1` |
| generator2 / consumer2 | Non-blocking with event retry | `f2` |
| generator3 / consumer3 | Capacity inspection | `f3` |

All generators write every **1 s**; all consumers read every **3 s** — so every FIFO fills up after 2 writes and the generator must wait for the consumer to drain a slot.

## Member Functions

### Blocking I/O

```cpp
f.write(v);    // blocks if FIFO is full until a slot is free
f.read(v);     // blocks if FIFO is empty until data is available
v = f.read();  // alternative read form returning the value
```

The simplest interface. The calling process suspends automatically if the operation cannot complete immediately. Used by generator1 and consumer1.

### Non-blocking I/O

```cpp
bool ok = f.nb_write(v);  // returns false immediately if full (does not block)
bool ok = f.nb_read(v);   // returns false immediately if empty (does not block)
```

Returns `false` without suspending if the operation cannot complete. The process must handle the failure itself — typically by waiting on an event and retrying:

```cpp
while (f2.nb_write(v) == false) {
    wait(f2.data_read_event());    // wait until a slot opens up
}
```

```cpp
while (f2.nb_read(v) == false) {
    wait(f2.data_written_event()); // wait until data arrives
}
```

### Events

```cpp
f.data_written_event()  // fires when data is written to the FIFO
f.data_read_event()     // fires when data is read from the FIFO
```

These events are what `sc_fifo` uses internally to implement its blocking operations. Exposing them allows non-blocking code to wait precisely for the condition it needs rather than polling.

### Capacity Inspection

```cpp
f.num_free()       // number of empty slots (write capacity remaining)
f.num_available()  // number of items waiting to be read
```

Used by generator3 and consumer3 to log the FIFO state before and after each operation. Together these two values always sum to the FIFO's total capacity.

## Blocking vs. Non-blocking — Comparison

| | Blocking (`write`/`read`) | Non-blocking (`nb_write`/`nb_read`) |
|---|---|---|
| **Full/empty FIFO** | Suspends automatically | Returns `false` immediately |
| **Code complexity** | Simple | Requires retry loop + event wait |
| **Control** | None — kernel decides when to resume | Full — process decides how to handle failure |
| **Use when** | Process must wait for the operation | Process can do something else or needs custom retry logic |

## Execution Timeline (f1 — blocking pair)

Generator writes every 1 s; consumer reads every 3 s; FIFO capacity is 2:

```
Time    Generator1 action       f1 state      Consumer1 action
──────────────────────────────────────────────────────────────
0 s     write(20) → ok          [20]
1 s     write(21) → ok          [20,21]
2 s     write(22) → BLOCKS      [20,21]       read() → 20  [21]
        (resumes after read)    [21,22]
3 s     write(23) → ok          [21,22,23]?   ...
```

After 2 writes the FIFO is full and generator1 blocks until consumer1 reads at 3 s, freeing a slot.

## Expected Output (first few lines)

```
@ time 0 s: GENERATOR 1 (using f1.write) writes 20
@ time 0 s: GENERATOR 2 (using f2.nb_write) writes 0
0 s: GENERATOR 3, before write, #free/#available=2/0
0 s: GENERATOR 3, after write, #free/#available=1/1
@ time 0 s: CONSUMER 1 (using f1.read) reads 20
0 s: CONSUMER 3, before read, #free/#available=1/1
0 s: CONSUMER 3, after read, #free/#available=2/0
@ time 1 s: GENERATOR 1 (using f1.write) writes 21
...
```

Consumer3 reads immediately at 0 s because generator3 also runs at 0 s and writes first. The `#free/#available` pair always sums to 2 (the FIFO capacity).

## `sc_fifo<T>` — Key Properties

- **Typed**: the template parameter `T` can be any copyable C++ type — `int`, `bool`, custom structs, etc.
- **Bounded**: capacity is fixed at construction and cannot change.
- **FIFO ordering**: values are read in the order they were written (first-in, first-out).
- **Thread-safe**: the channel handles all synchronization internally — no external mutex needed.
- **One writer, one reader**: `sc_fifo` is designed for a single producer and single consumer. Multiple concurrent writers or readers produce non-deterministic results.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_fifo<T>(N)` | Bounded FIFO of capacity N holding values of type T |
| `write(v)` | Blocking write — suspends if full |
| `read(v)` / `read()` | Blocking read — suspends if empty |
| `nb_write(v)` | Non-blocking write — returns `false` if full |
| `nb_read(v)` | Non-blocking read — returns `false` if empty |
| `data_written_event()` | Fires when a value is written; use to wake a waiting reader |
| `data_read_event()` | Fires when a value is read; use to wake a waiting writer |
| `num_free()` | Available write slots |
| `num_available()` | Values ready to read |

## Relation to Previous Examples

`sc_mutex` and `sc_semaphore` managed concurrent access but carried no payload. `sc_fifo` is the first primitive channel that **transfers data** — combining the flow control of a semaphore with typed value passing. The non-blocking pattern (`nb_write` + `data_read_event`) is a direct application of the event skills from examples 07–08: the process waits on a specific `sc_event` and retries when signalled.

## What Comes Next

`sc_fifo` works well for point-to-point streaming. The next channel type — **`sc_signal`** — is designed for hardware-style value propagation: it holds its last written value, notifies sensitive processes on change, and enforces single-driver discipline. Together with ports (`sc_in`, `sc_out`), it is the standard mechanism for connecting modules in a structural SystemC design.