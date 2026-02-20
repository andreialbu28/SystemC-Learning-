# SystemC Signals — Mixed Types & Method/Thread Interaction (`02_signal_example`)

This example extends the signal basics from `01` to show **three signals of different types** updating in lockstep with plain C++ variables across multiple delta cycles, and demonstrates how an `SC_METHOD` with static sensitivity automatically fires whenever any of those signals changes value.

## Overview

Real designs rarely use a single signal in isolation. This example uses three `sc_signal` channels of different types — `int`, a user-defined enum, and `std::string` — alongside corresponding plain C++ member variables. By stepping through several explicit delta cycles and observing when the method fires, the example makes the relationship between writes, delta updates, and sensitivity-triggered processes fully concrete.

## The Module

```cpp
// Plain C++ variables
int count;
color traffic_temp;
std::string message_temp;

// Signal channels
sc_signal<int>         count_sig;
sc_signal<color>       traffic_sig;
sc_signal<std::string> message_signal;
```

Each signal has a corresponding plain variable. The thread writes to both independently, letting the output reveal the difference between the two: plain variables update immediately; signals update after a delta.

An `SC_METHOD` is statically sensitive to all three signals:

```cpp
SC_METHOD(signal_example_method);
sensitive << count_sig << traffic_sig << message_signal;
dont_initialize();
```

`dont_initialize()` prevents the method from firing at time zero before any signal has been written. It will only trigger when a signal actually changes value.

## Delta-by-Delta Walkthrough

### Delta 1 — Initialization writes

```cpp
count_sig.write(10);
traffic_sig.write(BLACK);
message_signal.write("Hello");

count = 10;
traffic_temp = RED;
message_temp = "Whoa";

wait(SC_ZERO_TIME);
```

Three signals are written (pending) and three plain variables are set (immediate). The `wait(SC_ZERO_TIME)` triggers the update phase — all three signals commit their values and notify their change event. The `SC_METHOD` fires **once** (or up to three times — once per changed signal, depending on simulator implementation) with the newly committed values.

> Note the deliberate mismatch: `count_sig` is written `10` while `count` is also `10` — but `traffic_sig` is written `BLACK` while `traffic_temp` is set to `RED`, and `message_signal` is written `"Hello"` while `message_temp` is `"Whoa"`. The method output exposes both the signal value and the plain variable value side by side.

### Delta 2 — Update count

```cpp
count = 20;
count_sig.write(count);   // pending write: 20

// Reads BEFORE delta commit:
// count_sig.read() still returns 10 (previous committed value)
cout << "count is " << count << " count signal is " << count_sig.read();

wait(SC_ZERO_TIME);
```

`count` is `20` immediately. `count_sig.read()` still returns `10` — the write is pending. After `wait(SC_ZERO_TIME)` the update commits, the signal changes, and the method fires again.

### Delta 3 — Update traffic and message

```cpp
traffic_sig.write(traffic_temp = WHITE);
message_signal.write(message_temp = "Rev your engines");

wait(SC_ZERO_TIME);
```

Both plain variables and their signals are updated to the same values. Before the delta, `traffic_sig.read()` still returns the previous value. After `wait(SC_ZERO_TIME)` both signals commit and the method fires for each changed signal.

### Deltas 4 & 5 — No writes

```cpp
wait(SC_ZERO_TIME);  // delta 4
wait(SC_ZERO_TIME);  // delta 5
```

No writes are issued. No signals change. The `SC_METHOD` does **not** fire — it is only triggered by a value change, not by the passage of delta cycles.

## What the Method Sees

Each time `signal_example_method` fires it prints the current state of both the plain variables and the signals. Because the method runs **after** the delta commit, it always sees the freshly updated signal values. It also sees the plain variable values — which may differ from the signals if the thread has already advanced the plain variables ahead of the next signal write.

This side-by-side comparison is the pedagogical core of the example: it makes the timing relationship between plain variables, signal writes, delta commits, and method triggers visually explicit.

## `sc_signal<T>` with Custom Types

This example shows that `sc_signal` is fully generic:

```cpp
sc_signal<int>         count_sig;
sc_signal<color>       traffic_sig;   // user-defined enum
sc_signal<std::string> message_signal;
```

Any type `T` can be used as long as it:
- Supports copy assignment (`=`)
- Supports equality comparison (`==`) — used internally to detect value changes and decide whether to notify

If `T` does not define `operator==`, the signal cannot detect changes and will notify on every write, even if the value is the same.

## `dont_initialize()` with `SC_METHOD`

Without `dont_initialize()`, the method would fire during the initialization phase at time zero before any signals have been written — printing whatever garbage values happen to be in the signals. `dont_initialize()` ensures the method only reacts to genuine signal changes, giving a clean and predictable output.

## Expected Output Structure

```
INFO: Elaborating ...
INFO: Simulating ...
--- FIRST DELTA CYCLE ---
NOTE: signal_example_thread is initializing
NOTE: signal_example_method detected an EVENT      ← fires after delta 1 commit
...: count is 10 and count signal is 10
---: traffic_temp is 1 and traffic sig is 0        ← RED(1) vs BLACK(0)
---: message_temp is Whoa  and message signal is Hello
--- SECOND DELTA CYCLE ---
NOTE: signal_example_thread is done initializing
---: count is 20 count signal is 10                ← write pending
NOTE: signal_example_thread is waiting
NOTE: signal_example_method detected an EVENT      ← fires after delta 2 commit
...: count is 20 and count signal is 20
--- THIRD DELTA CYCLE ---
...
--- FORTH DELTA CYCLE ---
--- FIFTH DELTA CYCLE ---
NOTE: signal_ex_thread done
INFO: Post-processing ...
INFO: Simulation ... PASSED with 0 errors
```

The deliberate enum/string mismatches between plain variables and signals make the before/after delta difference easy to spot in the output.

## Key Concepts Reinforced and Extended

| Concept | Description |
|---|---|
| `sc_signal<T>` with enum / string | Any copyable, equality-comparable type works |
| Pending vs. committed value | `read()` returns committed value until delta passes |
| `SC_METHOD` + `sensitive <<` | Method fires automatically on any signal value change |
| `dont_initialize()` with `SC_METHOD` | Prevents spurious firing at time zero |
| Plain variable vs. signal timing | Plain variables update instantly; signals update at delta boundary |
| No-write delta cycles | Method does NOT fire if no signal changes value |
| `operator==` requirement | `sc_signal` uses equality to detect changes — required for custom types |

## Relation to Previous Sub-Example

`01_SystemC_signal_read_and_write` isolated the delta-deferred write mechanics with a single `int` signal and one thread. This example adds a second process (`SC_METHOD`) that reacts to changes, uses three signals of heterogeneous types, and deliberately mismatches plain variables from signal values to make the timing difference visible in the output.

## What Comes Next

The next sub-example introduces **`sc_signal` events** — specifically `value_changed_event()`, `posedge_event()`, and `negedge_event()` — which allow threads (not just methods) to wait on specific signal transitions.