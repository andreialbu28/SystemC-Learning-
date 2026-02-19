---
title: SystemC RTL as FMU — Methodology
description: Automated three-step methodology for wrapping SystemC RTL models as FMI 3.0 Co-Simulation FMUs.
tags:
  - FMI
  - RTL
  - SystemC
  - SDV
  - methodology
---

# SystemC RTL as FMU — Methodology

This page documents the methodology presented in:
> **Pollo, G., Albu, A. M., et al.** *Automatic Integration of SystemC in the FMI Standard for Software-Defined Vehicle Design.* FDL 2025. [IEEE](https://ieeexplore.ieee.org/document/11165273) 

---

## Motivation

SystemC RTL models provide **cycle-accurate, signal-level simulation** of hardware components. In an automotive Software-Defined Vehicle (SDV) context, these models represent ECU hardware blocks — memory controllers, bus interfaces, interrupt controllers, custom accelerators — that must be validated together with vehicle-level software, control algorithms, and physical plant models from completely different simulation domains.

The challenge is that SystemC RTL models are tightly coupled to the SystemC simulation kernel and its event-driven execution model. Exposing such a model to an FMI master requires solving three non-trivial problems:

1. **Interface extraction** — identifying which signals of the SystemC design should be exposed as FMI input/output variables
2. **Wrapper generation** — generating the FMI C-API implementation that drives the SystemC kernel from outside
3. **Time synchronization** — aligning the SystemC discrete-event time with the FMI master's step-based time model

The methodology of Pollo et al. addresses all three in a fully automated, non-intrusive pipeline.

---

## Three-Phase Workflow

```
            ┌─────────────────────────────────────────────────────────────┐
            │                                                             │
            │   Phase 1: Design Analysis                                  │
            │   ─────────────────────────                                 │
            │   • Parse SystemC source (SC_MODULE, sc_in, sc_out ports)   │
            │   • Identify top-level ports → future FMI variables         │
            │   • Classify direction: input / output / parameter          │
            │                                                             │
            │            ▼                                                │
            │                                                             │
            │   Phase 2: FMI Wrapper Generation                           │
            │   ────────────────────────────────                          │
            │   • Generate modelDescription.xml                           │
            │   • Implement FMI 3.0 C-API functions                       │
            │   • Map FMI step to SystemC sc_start(step_size)             │
            │   • Handle clock edges and reset signals                    │
            │                                                             │
            │            ▼                                                │
            │                                                             │
            │   Phase 3: Compilation & Validation                         │
            │   ──────────────────────────────────                        │
            │   • Compile SystemC + wrapper into shared library (.so/.dll)│
            │   • Package as .fmu archive                                 │
            │   • Validate with FMPy or FMI compliance checker            │
            │                                                             │
            └─────────────────────────────────────────────────────────────┘
```

### Phase 1 — Design Analysis

The toolchain parses the SystemC RTL source code to extract all **top-level port declarations** of the design-under-test. For each `sc_in<T>` and `sc_out<T>`, the tool determines:

- The signal name and C++ type (`bool`, `sc_uint<N>`, `sc_int<N>`, etc.)
- The direction (in → FMI `input`, out → FMI `output`)
- Whether it is a clock or reset signal (handled separately from data signals)

This analysis requires no manual annotation of the SystemC source — the tool reads standard, unmodified SystemC code.

### Phase 2 — FMI Wrapper Generation

The wrapper generation step produces two artifacts:

**`modelDescription.xml`** — the FMI interface contract. Every port extracted in Phase 1 becomes a `<ModelVariable>` with the appropriate `causality`, `variability`, and type annotation:

```xml
<!-- Example: a 32-bit data output signal -->
<ModelVariable name="data_out" valueReference="1"
               causality="output" variability="discrete">
    <UInt32 />
</ModelVariable>
```

**FMI C-API implementation** — a C wrapper file that implements the required FMI 3.0 functions. The three most critical functions are:

=== "`fmi3Instantiate`"
    Creates the SystemC simulation context, instantiates the top-level `SC_MODULE`, and connects internal signals. Equivalent to the static elaboration phase of a SystemC simulation.

    ```c
    fmi3Instance fmi3Instantiate(...) {
        // Create SystemC simulation context
        // Instantiate top-level SC_MODULE
        // Bind clock and reset signals
        // Initialize sc_start(0) to run elaboration
        return instance;
    }
    ```

=== "`fmi3DoStep`"
    The heart of the co-simulation. Called by the FMI master at each communication step. It transfers FMI input variables into the corresponding SystemC `sc_in` ports, advances the SystemC simulation by `communicationStepSize`, then reads back `sc_out` values into FMI output variables.

    ```c
    fmi3Status fmi3DoStep(fmi3Instance instance,
                          fmi3Float64 currentCommunicationPoint,
                          fmi3Float64 communicationStepSize, ...) {
        // Write FMI inputs → sc_signal/sc_in
        set_inputs(instance, inputValues);

        // Advance SystemC kernel
        sc_start(communicationStepSize, SC_SEC);

        // Read sc_out → FMI outputs
        get_outputs(instance, outputValues);

        return fmi3OK;
    }
    ```

=== "`fmi3Terminate` / `fmi3FreeInstance`"
    Gracefully stops the SystemC simulation kernel and frees all allocated resources.

### Phase 3 — Compilation and Packaging

The generated wrapper is compiled together with the original (unmodified) SystemC RTL sources and the SystemC library into a platform-specific shared library. This library, along with the `modelDescription.xml` and any resource files, is zipped into the final `.fmu` archive.

The resulting FMU can be dropped into any FMI 3.0-compatible master without any further modification.

---

## Non-Intrusiveness

A key design principle of this methodology is **non-intrusiveness**: the original SystemC source code is never modified. The wrapper is generated as an entirely separate layer that wraps the existing design externally. This means:

- Existing SystemC models can be wrapped as-is
- The RTL simulation behavior is identical whether run standalone or inside an FMU
- The methodology is applicable to any SC_MODULE-based design, regardless of complexity

---

## Interrupt Handling

One non-trivial challenge addressed in the methodology is **interrupt-like behavior**: SystemC designs may complete their step early (e.g., due to an exception or an internal event). The FMI `fmi3DoStep` function supports an `earlyReturnAllowed` flag for this purpose. The wrapper monitors internal SystemC events and sets the early-return time accordingly, preserving temporal accuracy across the FMI boundary.

---

## Prerequisites and Constraints

For the automated tool to work correctly, the SystemC design must satisfy a small set of constraints:

- The design must have a clearly identifiable **top-level SC_MODULE**
- Top-level ports must use standard SystemC port types (`sc_in`, `sc_out`, `sc_inout`)
- Signal types must be mappable to FMI variable types (primitives and fixed-width integers are supported out of the box)
- The design must compile cleanly with standard SystemC 2.3.3

!!! tip
    Designs using custom channel types or systemC objects deeply nested inside processes may require minor refactoring to expose a clean top-level interface. The methodology does not require changes to internal logic — only to the top-level port interface.

---

## Next Steps

- 📄 Read the full paper: [Pollo et al. (2025) →](pollo2025.md)
- 🔁 See the complementary TLM methodology: [SystemC TLM as FMU →](tlm-fmu-methodology.md)
- 🧪 See the cross-tool validation: [Co-simulation with Simulink →](co-simulation-simulink.md)