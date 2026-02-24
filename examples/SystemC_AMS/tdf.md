---
title: Timed Data Flow (TDF)
description: Hands-on walkthrough of SystemC AMS Timed Data Flow — the most commonly used AMS model of computation.
tags:
  - AMS
  - TDF
  - filter
  - example
---

# Timed Data Flow (TDF)

TDF is the most widely used SystemC AMS model of computation, and it's the best place to start. If you've ever written a signal-processing function that runs once per sample clock — a filter, a decimator, an ADC model — then you already understand the mental model. TDF just gives that pattern a first-class place in the SystemC world.

---

## The Core Idea

A TDF design is a **dataflow graph**: a network of modules where each module consumes a fixed number of input samples and produces a fixed number of output samples every timestep. The AMS scheduler analyses the graph at elaboration time, assigns a consistent global timestep to each cluster of connected modules, and then calls every module's `processing()` method in topological order on every tick.

```
   ┌──────────┐  sca_signal  ┌─────────────┐  sca_signal  ┌──────────┐
   │  Source  │─────────────►│  LP Filter  │─────────────►│  Tracer  │
   │ (TDF)    │              │   (TDF)     │              │  (TDF)   │
   └──────────┘              └─────────────┘              └──────────┘
        ▲
        │  sc_signal (digital)
        │
   ┌────┴─────┐
   │ SC_THREAD│  ← ordinary SystemC digital process
   │(stimulus)│
   └──────────┘
```

Key points:

- `processing()` replaces the sensitivity-list pattern of `SC_METHOD` / `SC_THREAD`
- Every port in a TDF cluster operates at the **same timestep** unless you explicitly configure rate conversion
- An `sca_tdf::sca_signal<T>` is the TDF equivalent of `sc_signal<T>` — it carries one sample per timestep

---

## Example: First-Order IIR Low-Pass Filter

This example implements a simple **first-order IIR (Infinite Impulse Response) low-pass filter** in TDF. The filter is driven by a digital step stimulus and the output is written to a CSV file for plotting.

The transfer function is:

$$y[n] = \alpha \cdot x[n] + (1 - \alpha) \cdot y[n-1]$$

where $\alpha = \frac{T_s}{T_s + \tau}$, $T_s$ is the sample period and $\tau$ is the RC time constant.

### File structure

```
examples/ams/tdf_lpf/
├── stimulus.h        # SC_THREAD that drives a step signal into AMS
├── lpf.h             # TDF low-pass filter module
├── tracer.h          # TDF module that writes output to CSV
└── main.cpp          # Top-level, wiring, sc_start
```

---

### `stimulus.h` — Digital stimulus driving the AMS chain

```cpp
// stimulus.h
// ---------------------------------------------------------------------------
// A plain SystemC SC_THREAD that produces a digital step signal.
// The sca_tdf::sca_de_out port bridges the digital (SC_THREAD) world and
// the AMS (TDF) world — it's the key "crossing point" between the two domains.
// ---------------------------------------------------------------------------
#pragma once
#include <systemc-ams>

SC_MODULE(Stimulus) {

    // sca_tdf::sca_de_out is the AMS-to-digital bridge port.
    // It wraps an sc_signal and makes it readable from a TDF module.
    sca_tdf::sca_de_out<double> out; // (1)

    SC_CTOR(Stimulus) {
        SC_THREAD(run);
    }

    void run() {
        out.write(0.0); // Initial value: 0V
        wait(10, SC_US); // Wait 10 microseconds

        out.write(1.0); // Step: 0V → 1V at t = 10 µs
        wait(SC_ZERO_TIME);

        // Hold at 1.0 for the rest of the simulation
        wait(); // suspend forever — sc_start() will end the simulation
    }
};
```

1. `sca_de_out` stands for "discrete-event output" — it converts a value written by a digital process into a TDF-readable signal. The TDF scheduler will sample it at each timestep.

---

### `lpf.h` — The TDF low-pass filter

```cpp
// lpf.h
// ---------------------------------------------------------------------------
// A first-order IIR low-pass filter implemented as a TDF module.
//
// The SCA_TDF_MODULE macro is shorthand for declaring a class that inherits
// from sca_tdf::sca_module. It works exactly like SC_MODULE — use SC_CTOR
// for the constructor.
// ---------------------------------------------------------------------------
#pragma once
#include <systemc-ams>

SCA_TDF_MODULE(LowPassFilter) { // (1)

    sca_tdf::sca_in<double>  in;  // One input sample consumed per timestep
    sca_tdf::sca_out<double> out; // One output sample produced per timestep

    // Filter parameter: RC time constant in seconds
    double tau;

    // Constructor: takes the time constant as a parameter
    LowPassFilter(sc_core::sc_module_name nm, double tau_seconds)
        : sca_tdf::sca_module(nm), in("in"), out("out"), tau(tau_seconds),
          y_prev(0.0) {}

    // set_attributes() is called by the AMS scheduler during elaboration.
    // This is where you set the module's timestep.
    void set_attributes() override {
        set_timestep(sca_core::sca_time(1.0, SC_US)); // (2)
    }

    // processing() is the TDF equivalent of an SC_METHOD body.
    // It is called once per timestep by the AMS scheduler.
    void processing() override {
        // Compute alpha from the current timestep and tau
        double ts = get_timestep().to_seconds(); // (3)
        double alpha = ts / (ts + tau);

        // IIR filter: y[n] = alpha*x[n] + (1-alpha)*y[n-1]
        double x = in.read();
        double y = alpha * x + (1.0 - alpha) * y_prev;

        out.write(y);
        y_prev = y; // Store for next timestep
    }

private:
    double y_prev; // Filter state: previous output sample
};
```

1. `SCA_TDF_MODULE(Name)` expands to `struct Name : public sca_tdf::sca_module`. It's a convenience macro — you can use `struct Name : public sca_tdf::sca_module` directly if you prefer.
2. `set_timestep()` tells the AMS scheduler how often `processing()` should be called. Here: every 1 µs, giving a 1 MHz sample rate.
3. `get_timestep()` retrieves the actual timestep after scheduler resolution — useful if multiple connected modules negotiate a common timestep.

---

### `tracer.h` — Writing output to CSV

```cpp
// tracer.h
// ---------------------------------------------------------------------------
// A TDF module that reads a signal and writes time/value pairs to a CSV file.
// SystemC AMS has a built-in sca_trace mechanism (similar to sc_trace for VCD),
// but a manual CSV writer is often more convenient for post-processing with
// Python/matplotlib.
// ---------------------------------------------------------------------------
#pragma once
#include <systemc-ams>
#include <fstream>
#include <string>

SCA_TDF_MODULE(Tracer) {

    sca_tdf::sca_in<double> in;

    Tracer(sc_core::sc_module_name nm, const std::string& filename)
        : sca_tdf::sca_module(nm), in("in") {
        file.open(filename);
        file << "time_us,value\n"; // CSV header
    }

    ~Tracer() {
        if (file.is_open()) file.close();
    }

    void processing() override {
        // get_time() returns the current simulation time as sca_core::sca_time
        double t_us = get_time().to_seconds() * 1e6;
        file << t_us << "," << in.read() << "\n";
    }

private:
    std::ofstream file;
};
```

---

### `main.cpp` — Top-level wiring

```cpp
// main.cpp
// ---------------------------------------------------------------------------
// Instantiates and connects all modules, then runs the simulation.
// The wiring pattern is identical to standard SystemC: declare signals,
// bind ports to signals.
// ---------------------------------------------------------------------------
#include "stimulus.h"
#include "lpf.h"
#include "tracer.h"

int sc_main(int, char**) {

    // ---------------------------------------------------------------------------
    // Signals
    // ---------------------------------------------------------------------------

    // sca_tdf::sca_signal<T> is the AMS equivalent of sc_signal<T>.
    // It carries one sample of type T per timestep between TDF modules.
    sca_tdf::sca_signal<double> sig_raw("sig_raw");   // stimulus → filter
    sca_tdf::sca_signal<double> sig_filtered("sig_filtered"); // filter → tracer

    // ---------------------------------------------------------------------------
    // Module instantiation
    // ---------------------------------------------------------------------------

    Stimulus  stim("stim");

    // tau = 5 µs → the filter's -3dB frequency is 1/(2π·5µs) ≈ 31.8 kHz
    LowPassFilter lpf("lpf", 5e-6);

    Tracer    trace_in("trace_in",  "output_raw.csv");
    Tracer    trace_out("trace_out", "output_filtered.csv");

    // ---------------------------------------------------------------------------
    // Port binding
    // ---------------------------------------------------------------------------

    // Bridge the SC_THREAD stimulus into the TDF domain
    stim.out(sig_raw); // (1)

    lpf.in(sig_raw);
    lpf.out(sig_filtered);

    trace_in.in(sig_raw);
    trace_out.in(sig_filtered);

    // ---------------------------------------------------------------------------
    // Run
    // ---------------------------------------------------------------------------

    // sc_start() launches both the SystemC and AMS schedulers.
    // The AMS scheduler will call processing() on each TDF module
    // every 1 µs for the duration of the simulation.
    sc_core::sc_start(100, SC_US);

    std::cout << "Simulation complete. Check output_raw.csv and output_filtered.csv\n";
    return 0;
}
```

1. Binding a `sca_de_out` to an `sca_tdf::sca_signal` is the standard way to drive a TDF chain from a digital process. The AMS scheduler handles the synchronisation automatically.

---

## Running the Example

```bash
# From the repo root
g++ -std=c++17 \
    -I./external/systemc-2.3.3/include \
    -I./external/systemc-ams-2.3/include \
    -L./external/systemc-2.3.3/lib-linux64 \
    -L./external/systemc-ams-2.3/lib-linux64 \
    -Wl,-rpath=./external/systemc-2.3.3/lib-linux64 \
    -Wl,-rpath=./external/systemc-ams-2.3/lib-linux64 \
    -lsystemc-ams -lsystemc -lm \
    examples/ams/tdf_lpf/main.cpp -o tdf_lpf

./tdf_lpf
```

Expected output:

```
        SystemC 2.3.3 (Build 2018-01-02 10:00:00)
        Copyright (c) 1996-2018 by all Contributors,
        ALL RIGHTS RESERVED
Simulation complete. Check output_raw.csv and output_filtered.csv
```

### Plotting with Python

```python
import pandas as pd
import matplotlib.pyplot as plt

raw      = pd.read_csv("output_raw.csv")
filtered = pd.read_csv("output_filtered.csv")

plt.figure(figsize=(10, 4))
plt.plot(raw["time_us"],      raw["value"],      label="Step input",       linestyle="--")
plt.plot(filtered["time_us"], filtered["value"], label="LPF output (τ=5µs)", linewidth=2)
plt.xlabel("Time (µs)")
plt.ylabel("Amplitude")
plt.title("TDF Low-Pass Filter — Step Response")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.savefig("tdf_lpf_step_response.png", dpi=150)
plt.show()
```

You should see the output rise smoothly from 0 to 1, with a time constant of ~5 µs.

---

## What to Remember

Three things make TDF different from a regular `SC_METHOD`:

1. **`set_attributes()` sets the timestep** — called once during elaboration. Set it here, not in the constructor.
2. **`processing()` is your sample-by-sample logic** — called every timestep by the AMS scheduler, in topological order through the dataflow graph.
3. **`sca_tdf::sca_signal<T>` connects TDF modules** — not `sc_signal<T>`. The two signal types are different and not directly interchangeable (use `sca_de_in` / `sca_de_out` to bridge them).

---

## Common Pitfalls

!!! warning "Setting timestep in the constructor"
    Don't call `set_timestep()` from the constructor. The AMS scheduler hasn't started yet. Always put it in `set_attributes()`.

!!! warning "Mixing sca_signal and sc_signal"
    You cannot bind an `sca_tdf::sca_in<T>` directly to an `sc_signal<T>`. Use `sca_tdf::sca_de_in<T>` on the TDF module side if the driving port is from a digital (SystemC) process.

!!! tip "Timestep negotiation"
    If two connected TDF modules request different timesteps, the AMS scheduler will try to find a consistent rate. It's safest to set the same timestep on every module in a cluster, or leave it unset on all but the source and let the scheduler propagate.

---

## Next Steps

- :material-chart-bell-curve: **[Linear Signal Flow (LSF) →](lsf.md)** — model continuous-time behaviour with gains and integrators
- :material-resistor: **[Electrical Linear Networks (ELN) →](eln.md)** — build an RC circuit from lumped elements
- :material-connection: **[Mixed-Signal Interfaces →](mixed-signal-interfaces.md)** — connect this TDF chain to a digital `SC_THREAD`