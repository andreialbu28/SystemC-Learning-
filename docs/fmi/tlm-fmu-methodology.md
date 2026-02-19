---
title: SystemC TLM as FMU — Methodology
description: Non-intrusive open-source toolchain for encapsulating SystemC TLM components as FMI 3.0 Co-Simulation FMUs.
tags:
  - FMI
  - TLM
  - SystemC
  - co-simulation
  - methodology
---

# SystemC TLM as FMU — Methodology

!!! abstract "Source paper"
    This page documents the methodology presented in:
    **Albu, A. M., Pollo, G., et al.** *Integrating SystemC TLM into FMI 3.0 Co-Simulations with an Open-Source Approach.* In press (2025). [arXiv:2508.20223](https://arxiv.org/abs/2508.20223)

---

## Motivation

At the TLM abstraction level, SystemC models communicate via **transactions** rather than individual signal toggling. A TLM target module receives read/write requests wrapped inside a `tlm_generic_payload` object, processes them, and returns a response — all within a single function call (`b_transport`) or a pair of non-blocking calls (`nb_transport_fw`/`nb_transport_bw`).

This transactional model is powerful for hardware/software co-design, but it poses unique challenges for FMI integration:

- FMI operates with **scalar or array variables** exchanged at fixed communication steps — there is no native concept of a transaction object
- The **TLM payload is a structured C++ object** (`tlm_generic_payload`) carrying address, data, byte enable, command, and response fields — these must be mapped to FMI variable types
- The SystemC event-driven kernel must be **time-synchronized** with the FMI master's step-based execution model without distorting the timing behavior of the TLM model

The methodology of Albu et al. solves all three challenges in a non-intrusive, fully automated way.

---

## Design Prerequisites

The toolchain targets **TLM target modules** — that is, SystemC modules that implement:

- `b_transport(tlm_generic_payload& payload, sc_time& delay)` — blocking transport interface
- Optionally: `nb_transport_fw(...)` — non-blocking forward path

The payload's data field is typed as a **user-defined struct** (e.g., `DataStruct`), which the toolchain parses to identify individual fields and their directions.

!!! example "Example: payload data struct"
    ```cpp
    struct DataStruct {
        uint32_t data_in;     // initiator → target (input)
        uint32_t data_out;    // target → initiator (output)
        uint8_t  command;     // initiator → target (input)
        uint32_t status;      // target → initiator (output)
    };
    ```
    The toolchain scans the `b_transport` and `nb_transport_fw` implementations to determine which fields are *read* (inputs from the initiator's perspective) and which are *written* (outputs produced by the target).

---

## Three-Phase Workflow

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  Phase 1: Design Selection and Analysis                         │
│  ──────────────────────────────────────                         │
│  • Parse SystemC TLM source code                                │
│  • Extract payload data struct fields                           │
│  • Classify fields as input (read by target) or                 │
│    output (written by target) via static analysis               │
│                                                                 │
│           ▼                                                     │
│                                                                 │
│  Phase 2: FMI Wrapper Generation                                │
│  ────────────────────────────────                               │
│  • Generate modelDescription.xml                                │
│    (one FMI variable per payload struct field)                  │
│  • Generate SystemC-FMI wrapper struct                          │
│  • Implement FMI 3.0 C-API:                                     │
│    - fmi3Instantiate → sc_start(0) elaboration                  │
│    - fmi3DoStep → set_and_send + sc_start(h) + retrieve_result  │
│    - fmi3Get/SetFloat64/UInt32/... → read/write wrapper fields  │
│                                                                 │
│           ▼                                                     │
│                                                                 │
│  Phase 3: Simulation and Validation                             │
│  ──────────────────────────────────                             │
│  • Compile: SystemC TLM + wrapper → shared library              │
│  • Package into .fmu archive                                    │
│  • Validate with FMPy or Simulink co-simulation master          │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### Phase 1 — Design Analysis

The toolchain parses the TLM target module's source code and extracts the payload data struct. For each field, it determines the **direction** by scanning the `b_transport` function body:

- If a field is *read* inside `b_transport` (e.g., `data_in = payload_ptr->data.data_in`), it is classified as an **FMI input** — it carries data from the FMI environment into the SystemC model
- If a field is *written* inside `b_transport` (e.g., `payload_ptr->data.data_out = result`), it is classified as an **FMI output** — it carries data from the SystemC model back to the FMI environment

### Phase 2 — FMI Wrapper Generation

**`modelDescription.xml`** maps each payload struct field to an FMI variable:

```xml
<!-- FMI input: data sent from master to TLM target -->
<ModelVariable name="data_in" valueReference="0"
               causality="input" variability="discrete">
    <UInt32 start="0"/>
</ModelVariable>

<!-- FMI output: result produced by TLM target -->
<ModelVariable name="data_out" valueReference="1"
               causality="output" variability="discrete">
    <UInt32 />
</ModelVariable>
```

**SystemC-FMI wrapper struct** bridges the FMI variable space and the TLM payload:

```cpp
// Generated: SystemC-FMI wrapper
struct FMIWrapper {
    // FMI variable storage
    uint32_t data_in  = 0;
    uint32_t data_out = 0;
    uint8_t  command  = 0;
    uint32_t status   = 0;

    // Pointer to top-level SystemC module
    TopLevel* top = nullptr;

    // Current simulation time (tracked across fmi3DoStep calls)
    double current_time = 0.0;
};
```

**`fmi3DoStep`** — the core execution function — implements the following sequence on each master-driven step:

```cpp
fmi3Status fmi3DoStep(fmi3Instance instance,
                      fmi3Float64 currentPoint,
                      fmi3Float64 stepSize, ...) {

    FMIWrapper* w = (FMIWrapper*) instance;

    // 1. Transfer FMI inputs → TLM payload
    w->top->set_and_send(w->data_in, w->command);

    // 2. Advance SystemC kernel by stepSize
    sc_start(stepSize, SC_SEC);    // (1)

    // 3. Retrieve TLM target outputs → FMI output storage
    w->top->retrieve_result(w->data_out, w->status);  // (2)

    // 4. Advance FMI time (handle early return if needed)
    w->current_time += stepSize;   // (3)

    return fmi3OK;
}
```

1. The SystemC kernel runs for exactly `stepSize` seconds, executing all pending TLM transactions within that window
2. The TLM target's output fields from the last transaction are copied into the FMI wrapper for the master to read via `fmi3GetUInt32` / `fmi3GetFloat64`
3. Early return (due to internal events or interrupt-like behavior) is handled by returning the actual elapsed time to the master

### Phase 3 — Compilation and Packaging

```bash
# Compile SystemC TLM model + generated wrapper
g++ -std=c++17 -shared -fPIC \
    -I./external/systemc-2.3.3/include \
    -L./external/systemc-2.3.3/lib-linux64 \
    -lsystemc \
    tlm_target.cpp fmi_wrapper_generated.cpp \
    -o binaries/linux64/my_tlm_model.so

# Package as FMU
zip -r my_tlm_model.fmu modelDescription.xml binaries/ resources/
```

---

## Time Synchronization

The most subtle challenge in SystemC TLM–to–FMI integration is **time synchronization**. SystemC uses a discrete-event simulation model where time advances only when events are scheduled. The FMI master drives a step-based model where `fmi3DoStep(h)` means "simulate for exactly `h` seconds".

The methodology resolves this by:

1. Mapping each `fmi3DoStep(h)` call directly to `sc_start(h, SC_SEC)` inside the wrapper
2. Using the `CommunicationStepSize` parameter from the `modelDescription.xml` to set a safe lower bound on the FMI step size, consistent with the TLM model's internal timing granularity
3. Monitoring `sc_time_stamp()` after each `sc_start()` to detect early completion and return the actual elapsed time to the FMI master via the `earlyReturn` mechanism introduced in FMI 3.0

---

## Non-Intrusiveness

Like the RTL methodology, this approach is **entirely non-intrusive**. The existing TLM source code is parsed but never modified. The wrapper is a generated, separate translation unit. This means:

- The TLM model can still be compiled and run as a standalone SystemC simulation, unchanged
- The FMU can be regenerated whenever the TLM model evolves, simply by re-running the toolchain
- Existing TLM IP can be wrapped for FMI co-simulation without involving the original authors

---

## Next Steps

- 📄 Read the full paper: [Albu et al. (2025) →](albu2025.md)
- 🔁 See the complementary RTL methodology: [SystemC RTL as FMU →](rtl-fmu-methodology.md)
- 🧪 See the cross-tool validation: [Co-simulation with Simulink →](co-simulation-simulink.md)