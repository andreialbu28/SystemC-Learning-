---
title: "01 — AMS Hello World"
description: Write your first TDF module, connect it to a sink, and run an AMS simulation alongside standard digital SystemC.
tags:
  - AMS
  - TDF
  - foundations
  - hello-world
---

# 01 — AMS Hello World

**Files:** `hello_tdf.h` · `print_sink.h` · `main.cpp`

This is the smallest possible SystemC AMS simulation: one TDF module produces
values on a fixed clock, one sink prints them. The goal is to understand what
`SCA_TDF_MODULE`, `set_attributes()`, and `processing()` are before adding any
real signal processing.

---

## Signal chain

```
HelloTDF (source)
  sca_tdf::sca_out<double>
        │
        │  sca_tdf::sca_signal<double>  "sig"
        │
  sca_tdf::sca_in<double>
PrintSink (sink)
```

---

## Key constructs introduced

| Construct | Role |
|-----------|------|
| `SCA_TDF_MODULE(Name)` | Declares a TDF module — the AMS counterpart of `SC_MODULE` |
| `SCA_CTOR(Name)` | Module constructor — works like `SC_CTOR` |
| `sca_tdf::sca_in<T>` / `sca_tdf::sca_out<T>` | TDF input/output ports |
| `sca_tdf::sca_signal<T>` | Channel connecting TDF ports — not interchangeable with `sc_signal<T>` |
| `set_attributes()` | Elaboration callback — the only place to call `set_timestep()` |
| `set_timestep(sca_core::sca_time(...))` | Sets how often `processing()` is called |
| `processing()` | The per-sample computation — called by the AMS scheduler every timestep |

---

## `hello_tdf.h` — The source module

```cpp
--8<-- "examples/SystemC_AMS/20_AMS_Foundations/01_ams_hello_world/hello_tdf.h"
```

### What the callbacks do

`set_attributes()` is called once during elaboration — before any simulation
time runs. This is where the AMS scheduler learns the timestep. Setting it
to 1 µs means `processing()` will be called at t = 0, 1, 2, 3, 4 µs.

`processing()` is called by the AMS scheduler on every tick. It runs to
completion (like `SC_METHOD`) — there is no `wait()`, no suspension.

!!! warning "Don't call set_timestep() from the constructor"
    The AMS scheduler hasn't started during construction. `set_timestep()`
    belongs in `set_attributes()` only.

---

## `print_sink.h` — The sink module

```cpp
--8<-- "examples/SystemC_AMS/20_AMS_Foundations/01_ams_hello_world/print_sink.h"
```

The sink omits `set_timestep()`. When a TDF module has no explicit timestep,
the AMS scheduler **propagates** the timestep from the driving module through
the connected signal. The sink automatically runs at the same rate as the
source. This is the safe default for sinks and intermediate stages.

---

## `main.cpp` — Wiring and running

```cpp
--8<-- "examples/SystemC_AMS/20_AMS_Foundations/01_ams_hello_world/main.cpp"
```

---

## Expected output

```
=== AMS Hello World ===

[HelloTDF] t =          0 s  sample[0] = 0.000
[HelloTDF] t =       1 us  sample[1] = 0.100
[PrintSink]  t =       1 us  value = 0.100
[HelloTDF] t =       2 us  sample[2] = 0.200
[PrintSink]  t =       2 us  value = 0.200
[HelloTDF] t =       3 us  sample[3] = 0.300
[PrintSink]  t =       3 us  value = 0.300
[HelloTDF] t =       4 us  sample[4] = 0.400
[PrintSink]  t =       4 us  value = 0.400

Simulation finished at t = 5 us
```

!!! note "Why does HelloTDF print at t=0 but PrintSink starts at t=1 µs?"
    At t=0 the source writes sample[0]=0.0 to `sig`. The sink reads the signal
    at the *end* of the first timestep (t=1 µs), which is when `sig` carries
    the value written at t=0. This one-timestep pipeline latency is normal
    in TDF dataflow — it is not a bug.

---

## Build and run

```bash
g++ -std=c++17 \
    -I./external/systemc-2.3.3/include \
    -I./external/systemc-ams-2.3/include \
    -L./external/systemc-2.3.3/lib-linux64 \
    -L./external/systemc-ams-2.3/lib-linux64 \
    -Wl,-rpath=./external/systemc-2.3.3/lib-linux64 \
    -Wl,-rpath=./external/systemc-ams-2.3/lib-linux64 \
    -lsystemc-ams -lsystemc -lm \
    examples/SystemC_AMS/20_AMS_Foundations/01_ams_hello_world/main.cpp \
    -o ams_hello_world

./ams_hello_world
```

---

## Summary

Three things that make a TDF module different from an `SC_MODULE`:

1. Use `SCA_TDF_MODULE` and `SCA_CTOR` instead of `SC_MODULE` and `SC_CTOR`
2. Put `set_timestep()` in `set_attributes()`, never in the constructor
3. Write your computation in `processing()` — it runs every timestep, no sensitivity list needed

Next: [02 — Time in AMS →](02_time_in_ams.md) — see exactly how the AMS timestep clock differs from the digital event-driven kernel.