# SystemC Time

This example explores **how SystemC represents and manages simulation time** — configuring the time resolution and default time unit, converting between time scales, and formatting simulation time for human-readable output.

## Overview

Time is a first-class concept in SystemC. Unlike plain C++ where time is just a number you track yourself, the SystemC kernel maintains a global simulation clock with a fixed resolution and provides the `sc_time` type to express durations and timestamps precisely. Before any simulation runs, that time system must be configured correctly.

## Project Structure

```
├── include/
│   └── time_utils.h        # Function declarations
├── src/
│   ├── time_utils.cpp      # Time utility implementations
│   └── main.cpp            # sc_main entry point
```

## Time System Configuration

### Resolution vs. Default Time Unit

Two global settings govern how SystemC handles time:

| Setting | Function | What it controls |
|---|---|---|
| **Time resolution** | `sc_set_time_resolution()` | The smallest representable time quantum — all `sc_time` values are internally stored as integer multiples of this value |
| **Default time unit** | `sc_set_default_time_unit()` | The unit used when printing or converting time via `to_default_time_units()` |

Both must be set **before** any `sc_time` objects are constructed and before `sc_start()` is called. This example sets them at the very beginning of `sc_main` via `configure_time_system()`:

```cpp
sc_set_time_resolution(1, SC_FS);   // 1 femtosecond — highest precision
sc_set_default_time_unit(1, SC_SEC); // display in seconds
```

Setting the resolution to femtoseconds allows the simulation to represent time spans from 1 fs all the way up to several thousand seconds without loss of precision — at the cost of requiring large internal integers. This is the finest granularity SystemC supports.

### Suppressing the Deprecation Warning

```cpp
sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
```

In SystemC 3.x, `sc_set_time_resolution()` is marked as deprecated in favour of setting resolution via elaboration-time configuration. This line silences that warning so the output stays clean. In production code you would use the recommended configuration method for your SystemC version.

## Time Units

SystemC defines six time unit constants:

| Constant | Scale | Value |
|---|---|---|
| `SC_SEC` | Seconds | 1 s |
| `SC_MS` | Milliseconds | 1 × 10⁻³ s |
| `SC_US` | Microseconds | 1 × 10⁻⁶ s |
| `SC_NS` | Nanoseconds | 1 × 10⁻⁹ s |
| `SC_PS` | Picoseconds | 1 × 10⁻¹² s |
| `SC_FS` | Femtoseconds | 1 × 10⁻¹⁵ s |

An `sc_time` object is constructed by pairing a value with one of these constants:

```cpp
sc_time(1, SC_NS)   // one nanosecond
sc_time(500, SC_MS) // 500 milliseconds
```

## Time Conversion

`print_time_conversions()` demonstrates `to_default_time_units()`, which expresses any `sc_time` value as a `double` in terms of the configured default time unit (here, seconds):

```cpp
sc_time(1, SC_SEC).to_default_time_units()  // → 1.0
sc_time(1, SC_MS).to_default_time_units()   // → 0.001
sc_time(1, SC_NS).to_default_time_units()   // → 1e-9
sc_time(1, SC_FS).to_default_time_units()   // → 1e-15
```

This is useful for logging, assertions, or any place where you need a plain numeric value rather than an `sc_time` object.

## Running the Simulation

```cpp
sc_start(10000, SC_SEC);
```

The kernel advances simulation time by 10 000 seconds with no processes registered — the kernel simply moves time forward and returns. The current timestamp is then retrieved and formatted:

```cpp
double t = sc_time_stamp().to_seconds();

int hours   = static_cast<int>(t) / 3600;
int minutes = (static_cast<int>(t) % 3600) / 60;
int seconds = static_cast<int>(t) % 60;
```

`sc_time_stamp()` returns the current simulation time as an `sc_time` object. `.to_seconds()` converts it to a `double` in SI seconds, which can then be decomposed with standard integer arithmetic into hours, minutes, and seconds.

## Expected Output

```
=== Time System Configured ===
Resolution: 1 femtosecond (1e-15 s)
Default unit: 1 second

=== Time Unit Conversions ===
1 second = 1 seconds (default unit)
1 second = 0.001 seconds (1e-3)
1 second = 1e-06 seconds (1e-6)
1 second = 1e-09 seconds (1e-9)
1 second = 1e-12 seconds (1e-12)
1 second = 1e-15 seconds (1e-15)

=== Running Simulation for 10000 seconds ===
Simulation time: 10000 seconds
Formatted: 2 hours, 46 minutes, 40 seconds
```

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_set_time_resolution()` | Sets the smallest representable time quantum globally |
| `sc_set_default_time_unit()` | Sets the unit used by `to_default_time_units()` |
| `sc_time(value, unit)` | Constructs a time duration from a value and a unit constant |
| `to_default_time_units()` | Returns the time as a `double` in the configured default unit |
| `to_seconds()` | Returns the time as a `double` in SI seconds, regardless of default unit |
| `sc_time_stamp()` | Returns the current simulation time as an `sc_time` object |
| `sc_report_handler` | SystemC's reporting and warning control mechanism |

## Relation to Previous Examples

Previous examples used time implicitly — passing `SC_SEC` or `SC_MS` directly to `wait()` and `sc_start()`. This example makes the time system itself the subject, showing the configuration that underlies all of those calls and how to work with `sc_time` values programmatically.

## What Comes Next

With time handling understood, the next step is **signals and ports** — connecting modules together so that value changes propagate between them across simulation time.