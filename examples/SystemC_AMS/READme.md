# SystemC AMS Tutorial — Complete Learning Guide

Welcome to the SystemC AMS (Analog/Mixed-Signal) tutorial series. This guide is designed for someone who has a solid foundation in SystemC digital modeling (processes, signals, TLM) and wants to extend their knowledge to model analog, mixed-signal, and continuous-time systems within the same simulation framework.

---

## What Is SystemC AMS and Why Does It Matter?

Modern electronic systems are never purely digital. A smartphone SoC includes RF front-ends, ADCs, DACs, power management circuits, MEMS sensors, and analog filters — all interacting with digital processing cores. Traditional approaches model these two worlds separately: SPICE for analog, HDL for digital, with manual effort to co-simulate them.

**SystemC AMS** bridges this gap by extending SystemC with models of computation (MoCs) tailored for analog and mixed-signal behavior. It allows you to:

- Model analog and digital subsystems in a **single simulation environment**
- Explore mixed-signal architectures early in the design flow, before transistor-level circuits exist
- Validate digital signal processing algorithms against realistic analog impairments
- Create executable specifications for analog IP that can be refined toward RTL or SPICE
- Run **much faster** than SPICE while capturing the key behavioral characteristics of analog systems

SystemC AMS 2.0 is standardized by the Accellera Systems Initiative and is part of the broader IEEE 1666 family.

---

## The Three Models of Computation

Unlike digital SystemC (which has one execution model — the delta-cycle event-driven kernel), SystemC AMS provides **three distinct models of computation**, each suited to a different type of signal and system:

| MoC | Full Name | Best for |
|---|---|---|
| **TDF** | Timed Data Flow | Discrete-time signal processing, DSP algorithms, dataflow systems |
| **LSF** | Linear Signal Flow | Continuous-time linear analog circuits, transfer functions, filters |
| **ELN** | Electrical Linear Network | Netlist-style circuit models, RLC networks, operational amplifiers |

These three MoCs can be used together in the same design and can interact with each other and with standard digital SystemC through dedicated converter modules.

Understanding which MoC to use for a given design task is the central skill this tutorial develops.

---

## Prerequisites

| Topic | Where covered |
|---|---|
| SystemC modules, processes, events | RTL series examples 01–10 |
| `sc_signal`, ports, sensitivity | RTL series example 14 |
| Basic digital signal processing concepts (sampling, filtering) | General DSP knowledge |
| C++ templates and operator overloading | General C++ knowledge |
| Basic circuit theory (Ohm's law, KVL, KCL, RC/RL/LC circuits) | For ELN section |
| Laplace transforms and transfer functions | For LSF section |

---

## Tutorial Structure and Timeline

---

### Section 1 — `20_AMS_Foundations` — Setup and Concepts

**Goal:** Understand what SystemC AMS adds to SystemC, how the three MoCs relate to each other, and how AMS simulates time differently from digital SystemC.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_ams_hello_world` | First AMS module | How to write a minimal TDF module, register a processing function with `SCA_CTOR`, and run an AMS simulation alongside digital SystemC |
| `02_time_in_ams` | Simulation time models | The difference between digital delta-cycle time and AMS timestep time; how `set_timestep()` controls TDF execution rate |
| `03_ams_trace` | Signal tracing | How to use `sca_trace_file` and `sca_trace()` to record AMS signals to VCD or tabular files for waveform viewing |

**Key concepts introduced:**

`sca_tdf::sca_module`, `SCA_CTOR`, `sca_tdf::sca_in<T>`, `sca_tdf::sca_out<T>`, `sca_core::sca_time`, `set_timestep()`, `initialize()`, `processing()`, `sca_trace_file`, `sca_trace()`, `sca_close_vcd_trace_file()`

**By the end of Section 1** you will have a working AMS simulation environment and understand the fundamental difference between event-driven digital time and timestep-driven analog time.

---

### Section 2 — `21_TDF_Timed_Data_Flow` — Discrete-Time Signal Processing

**Goal:** Master the Timed Data Flow MoC — the most commonly used AMS model of computation, suited for DSP algorithms, sampled-data systems, and behavioral models of mixed-signal blocks.

TDF executes modules in a **static schedule** determined at elaboration time. Each module processes a fixed number of samples per timestep. There are no events, no dynamic scheduling — just a regular, pipelined dataflow that maps naturally onto sampled-data systems.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_tdf_source` | Signal sources | How to write TDF source modules: sine generators, ramps, noise sources, pulse generators |
| `02_tdf_sink` | Signal sinks | How to write TDF sink modules that consume signals for logging, assertion checking, or driving external files |
| `03_tdf_gain` | Basic processing | The fundamental TDF processing pattern: read input, compute, write output within a single `processing()` call |
| `04_tdf_delay` | Sample delays | How to introduce sample-level delays using `set_delay()` — modeling pipeline stages and feedback paths |
| `05_tdf_decimation` | Rate conversion (down) | How decimation (downsampling) works in TDF using `set_rate()` — consuming N samples per timestep |
| `06_tdf_interpolation` | Rate conversion (up) | How interpolation (upsampling) works — producing M samples per timestep |
| `07_tdf_fir_filter` | FIR filter | Implementing a complete Finite Impulse Response filter in TDF — the canonical DSP example |
| `08_tdf_feedback` | Feedback loops | How TDF handles algebraic loops and feedback using delays and the `sca_tdf::sca_signal` channel |

**Key concepts introduced:**

`processing()`, `initialize()`, `set_timestep()`, `set_rate()`, `set_delay()`, `read()`, `write()`, `sca_tdf::sca_signal<T>`, `sca_tdf::sca_de::sca_in<T>` (DE-to-TDF converter port), static schedule, dataflow graph, rate consistency, sample delay vs. time delay

**By the end of Section 2** you will be able to model complete sampled-data signal processing chains — sources, filters, decimators, interpolators — and understand how TDF's static schedule makes it fast and predictable.

---

### Section 3 — `22_LSF_Linear_Signal_Flow` — Continuous-Time Linear Systems

**Goal:** Model continuous-time analog behavior using the Linear Signal Flow MoC — ideal for transfer functions, filters, control loops, and any system that can be described by linear differential equations.

LSF operates in **continuous time** using Laplace-domain descriptions. You describe systems as signal flow graphs — gains, integrators, summers, transfer functions — and the solver handles the mathematics automatically. No numerical integration code, no state-space matrices to manage manually.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_lsf_gain_and_summer` | Basic LSF elements | The fundamental LSF primitives: `sca_lsf::sca_gain`, `sca_lsf::sca_add`, `sca_lsf::sca_sub` |
| `02_lsf_integrator` | Integration and differentiation | `sca_lsf::sca_integ` and `sca_lsf::sca_deriv` — the continuous-time building blocks |
| `03_lsf_transfer_function` | Transfer functions | How to specify any linear transfer function directly in Laplace domain using `sca_lsf::sca_tf` |
| `04_lsf_rc_filter` | RC low-pass filter | Building a first-order RC filter as an LSF signal flow graph and comparing to the transfer function form |
| `05_lsf_pid_controller` | PID controller | A complete proportional-integral-derivative controller — the canonical control engineering example in LSF |
| `06_lsf_tdf_interface` | LSF ↔ TDF bridge | How to convert between LSF continuous-time signals and TDF discrete-time samples using `sca_lsf::sca_de_gain` and converter ports |

**Key concepts introduced:**

`sca_lsf::sca_signal`, `sca_lsf::sca_gain`, `sca_lsf::sca_add`, `sca_lsf::sca_sub`, `sca_lsf::sca_integ`, `sca_lsf::sca_deriv`, `sca_lsf::sca_tf` (numerator/denominator polynomial), `sca_lsf::sca_source`, `sca_lsf::sca_sink`, Laplace domain representation, signal flow graph topology, LSF-to-TDF and TDF-to-LSF converter ports

**By the end of Section 3** you will be able to model any linear continuous-time analog system — filters, amplifiers, control loops — directly in the Laplace domain without writing numerical solvers.

---

### Section 4 — `23_ELN_Electrical_Linear_Network` — Circuit-Level Modeling

**Goal:** Model analog circuits at the netlist level using the Electrical Linear Network MoC — the closest SystemC AMS gets to SPICE without actually being SPICE.

ELN describes circuits as networks of nodes connected by primitive elements (resistors, capacitors, inductors, voltage sources, current sources, controlled sources). The solver applies Kirchhoff's laws automatically using Modified Nodal Analysis (MNA). This is ideal when you know the circuit topology but want behavioral simulation speed rather than transistor-level accuracy.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_eln_resistor_divider` | Basic ELN netlist | How to declare nodes, instantiate ELN primitives (`sca_eln::sca_r`), and connect them to form a resistor voltage divider |
| `02_eln_rc_circuit` | RC circuit | Adding capacitors (`sca_eln::sca_c`) — the first circuit with dynamic (time-varying) behavior |
| `03_eln_rlc_circuit` | RLC resonator | Inductors (`sca_eln::sca_l`), resonance, and transient response |
| `04_eln_voltage_source` | Excitation sources | Voltage sources (`sca_eln::sca_vsource`), current sources (`sca_eln::sca_isource`), sinusoidal and step excitation |
| `05_eln_controlled_source` | Dependent sources | Voltage-controlled voltage source (VCVS), current-controlled current source — modeling amplifier gain |
| `06_eln_opamp` | Operational amplifier | Building a behavioral op-amp model from ELN primitives — inverting amplifier, integrator configurations |
| `07_eln_tdf_interface` | ELN ↔ TDF bridge | Converting between ELN node voltages/currents and TDF signals using `sca_eln::sca_tdf_vsource` and `sca_eln::sca_tdf_vout` |

**Key concepts introduced:**

`sca_eln::sca_node`, `sca_eln::sca_node_ref` (ground), `sca_eln::sca_r`, `sca_eln::sca_c`, `sca_eln::sca_l`, `sca_eln::sca_vsource`, `sca_eln::sca_isource`, `sca_eln::sca_vcvs`, `sca_eln::sca_cccs`, Modified Nodal Analysis, netlist connectivity, ELN-to-TDF converter elements, Kirchhoff's laws in simulation

**By the end of Section 4** you will be able to model analog circuits from schematics directly in SystemC AMS, simulate their transient response, and interface those circuits with digital and dataflow processing.

---

### Section 5 — `24_AMS_Mixed_Signal` — Connecting the Worlds

**Goal:** Learn how AMS modules connect to each other across MoC boundaries, and how the entire AMS world interfaces with the standard digital SystemC (DE — Discrete Event) domain.

This is where SystemC AMS becomes truly powerful: a complete mixed-signal system with digital logic, sampled-data processing, continuous-time analog circuits, and the converters that tie them all together.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_de_tdf_interface` | Digital ↔ TDF | How `sca_tdf::sca_de::sca_in<T>` and `sca_tdf::sca_de::sca_out<T>` converter ports bridge `sc_signal` and TDF signals |
| `02_de_lsf_interface` | Digital ↔ LSF | Driving LSF systems from digital SystemC signals and reading LSF outputs back into digital processes |
| `03_de_eln_interface` | Digital ↔ ELN | Connecting digital control signals to ELN circuit elements — modeling digitally-controlled analog circuits |
| `04_adc_model` | ADC behavioral model | A complete analog-to-digital converter model: ELN anti-alias filter → TDF sampler → digital output |
| `05_dac_model` | DAC behavioral model | A complete digital-to-analog converter model: digital input → TDF interpolation → LSF reconstruction filter |
| `06_mixed_signal_system` | Full mixed-signal loop | A complete closed-loop system: digital controller → DAC → analog plant (ELN) → ADC → digital controller |

**Key concepts introduced:**

DE-to-TDF converter ports, TDF-to-DE converter ports, DE-to-LSF and LSF-to-DE interfaces, DE-to-ELN interfaces, multi-MoC design composition, time alignment between domains, ADC/DAC behavioral modeling, closed-loop mixed-signal simulation

**By the end of Section 5** you will be able to build a complete mixed-signal virtual prototype — digital, sampled-data, and continuous-time components all simulating together.

---

### Section 6 — `25_AMS_Advanced` — Nonlinearity, Verification, and Real-World Modeling

**Goal:** Go beyond linear models to handle the nonlinear and statistical behaviors that make real analog design challenging, and learn techniques for verifying AMS designs.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_nonlinear_tdf` | Nonlinear TDF modules | Modeling nonlinear functions (saturation, clipping, square law) in TDF using arbitrary C++ computation in `processing()` |
| `02_noise_modeling` | Noise sources | Adding thermal noise, flicker noise, and quantization noise to TDF and ELN models |
| `03_impairment_modeling` | Analog impairments | Modeling real-world RF/analog impairments: gain compression, IQ mismatch, phase noise, DC offset |
| `04_parametric_sweep` | Parameter sweeps | Running multiple simulations across a parameter range to characterize a design — bandwidth vs. gain, noise figure vs. bias |
| `05_ams_assertions` | AMS verification | Writing SystemC AMS checkers and assertions — verifying signal amplitude, frequency, settling time, and SNR |
| `06_rf_link_budget` | RF link budget model | A complete RF receive chain model: antenna → LNA → mixer → IF filter → ADC — demonstrating realistic system-level AMS modeling |

**Key concepts introduced:**

Nonlinear `processing()` functions, `sca_tdf` noise injection, statistical distributions in AMS, gain compression (1dB compression point), IQ imbalance, phase noise, parametric simulation loops, AMS signal checkers, SNR computation, RF system-level modeling, cascaded noise figure

**By the end of Section 6** you will be able to model realistic, non-ideal analog systems, inject impairments, and verify that your design meets its specification.

---

## Learning Path Summary

```
Section 1 — Foundations
┌──────────────────────────────────┐
│ 01 Hello world                   │
│ 02 Time models                   │ ◄── Start here
│ 03 Tracing                       │
└──────────────────────────────────┘
                │
     ┌──────────┴──────────┐
     ▼                     ▼
Section 2 — TDF      Section 3 — LSF
┌────────────────┐    ┌────────────────┐
│ Sources/sinks  │    │ Gain/sum       │
│ Gain/delay     │    │ Integrators    │
│ Rate change    │    │ Transfer fn    │
│ FIR filter     │    │ RC/PID         │
│ Feedback       │    │ TDF interface  │
└────────────────┘    └────────────────┘
          │                  │
          └────────┬─────────┘
                   ▼
          Section 4 — ELN
          ┌──────────────────────────┐
          │ Resistor / RC / RLC      │
          │ Sources / controlled src │
          │ Op-amp / TDF interface   │
          └──────────────────────────┘
                   │
                   ▼
          Section 5 — Mixed Signal
          ┌──────────────────────────┐
          │ DE ↔ TDF / LSF / ELN    │
          │ ADC model                │
          │ DAC model                │
          │ Full closed-loop system  │
          └──────────────────────────┘
                   │
                   ▼
          Section 6 — Advanced
          ┌──────────────────────────┐
          │ Nonlinearity             │
          │ Noise & impairments      │
          │ Verification             │
          │ RF link budget           │
          └──────────────────────────┘
```

---

## MoC Selection Guide

Use this table when deciding which AMS model of computation to use for a given design block:

| Design block | Recommended MoC | Reason |
|---|---|---|
| DSP algorithm (FIR, FFT, equalizer) | TDF | Naturally sampled-data; efficient static schedule |
| ADC / DAC (behavioral) | TDF | Sampling and reconstruction are discrete-time operations |
| PLL (behavioral, linear model) | LSF | Phase/frequency domain; linear continuous-time |
| Analog filter (behavioral) | LSF or ELN | LSF for transfer function; ELN for circuit topology |
| Amplifier (behavioral) | LSF (linear) or TDF (nonlinear) | LSF if linear; TDF for saturation/compression |
| RLC circuit | ELN | Direct netlist mapping |
| Op-amp circuit | ELN | Netlist with controlled sources |
| RF receiver chain | TDF + LSF | TDF for DSP; LSF for filters and matching |
| Control loop | LSF (plant) + TDF (controller) | Continuous plant, sampled controller |
| Sensor interface | ELN (sensor) + TDF (readout) | Circuit model of sensor, digital readout |

---

## Estimated Study Time

| Section | Sub-examples | Estimated time |
|---|---|---|
| 20 — Foundations | 3 | 2–3 hours |
| 21 — TDF | 8 | 6–8 hours |
| 22 — LSF | 6 | 5–6 hours |
| 23 — ELN | 7 | 6–8 hours |
| 24 — Mixed Signal | 6 | 6–8 hours |
| 25 — Advanced | 6 | 6–8 hours |
| **Total** | **36** | **31–41 hours** |

ELN and mixed-signal sections take longer because they require circuit theory background and the concepts are more layered. Allow extra time for these if circuits are less familiar.

---

## Key SystemC AMS Reference — At a Glance

### Port Types by MoC

| MoC | Input port | Output port | Signal channel |
|---|---|---|---|
| TDF | `sca_tdf::sca_in<T>` | `sca_tdf::sca_out<T>` | `sca_tdf::sca_signal<T>` |
| LSF | `sca_lsf::sca_in` | `sca_lsf::sca_out` | `sca_lsf::sca_signal` |
| ELN | `sca_eln::sca_node` | `sca_eln::sca_node` | `sca_eln::sca_node` |
| DE↔TDF | `sca_tdf::sca_de::sca_in<T>` | `sca_tdf::sca_de::sca_out<T>` | `sc_signal<T>` (DE side) |

### TDF Module Callback Functions

| Function | When called | Purpose |
|---|---|---|
| `set_attributes()` | Elaboration | Set `set_timestep()`, `set_rate()`, `set_delay()` |
| `initialize()` | Before simulation starts | Set initial output values and state |
| `processing()` | Every timestep | Main computation — read inputs, write outputs |
| `ac_processing()` | AC analysis (optional) | Frequency-domain behavior for AC simulation |

### Key ELN Primitive Elements

| Class | Models | Key parameters |
|---|---|---|
| `sca_eln::sca_r` | Resistor | `value` (Ω) |
| `sca_eln::sca_c` | Capacitor | `value` (F) |
| `sca_eln::sca_l` | Inductor | `value` (H) |
| `sca_eln::sca_vsource` | Voltage source | `init_value`, `offset`, `amplitude`, `frequency` |
| `sca_eln::sca_isource` | Current source | Same as vsource |
| `sca_eln::sca_vcvs` | Voltage-controlled voltage source | `gain` |
| `sca_eln::sca_vccs` | Voltage-controlled current source | `value` (S) |
| `sca_eln::sca_tdf_vsource` | TDF-driven voltage source | Bridges TDF → ELN |
| `sca_eln::sca_tdf_vout` | Voltage output to TDF | Bridges ELN → TDF |

### Key LSF Primitive Elements

| Class | Models | Key parameters |
|---|---|---|
| `sca_lsf::sca_gain` | Scalar gain | `k` |
| `sca_lsf::sca_add` | Two-input adder | — |
| `sca_lsf::sca_sub` | Two-input subtractor | — |
| `sca_lsf::sca_integ` | Integrator (1/s) | `k`, `t` (time constant) |
| `sca_lsf::sca_deriv` | Differentiator (s) | `k`, `t` |
| `sca_lsf::sca_tf` | Arbitrary transfer function | `num[]`, `den[]` (polynomial coefficients) |
| `sca_lsf::sca_source` | Signal source | `init_value`, `offset`, `amplitude` |

---

## How AMS Simulation Time Works

This is the most important conceptual difference from digital SystemC — and the most common source of confusion.

**Digital SystemC** uses an event-driven, delta-cycle model. Time advances only when events are scheduled. There is no concept of "continuous time."

**SystemC AMS** introduces a parallel timestep-driven execution alongside the event-driven digital kernel:

```
Digital (DE) time:  0──────────────────────────────────► (event-driven)
                    │         │              │
AMS timestep time:  ▼         ▼              ▼
                    0──Δt──2Δt──3Δt──4Δt──5Δt──►  (regular, fixed steps)
```

TDF, LSF, and ELN modules execute at regular timestep intervals. The AMS kernel synchronizes with the DE kernel at each timestep boundary, allowing DE signals to drive AMS inputs and AMS outputs to update DE signals — but only at these synchronization points.

This means:
- AMS signals change continuously between synchronization points (from the analog perspective)
- Digital signals only update at event times (from the DE perspective)
- The `set_timestep()` setting controls accuracy vs. speed — smaller timestep = more accurate, slower

---

## Further Reading

After completing this tutorial series:

- **IEEE Std 1666.1-2016** — The official SystemC AMS standard
- **"SystemC AMS Extensions" by Accellera** — The language reference manual (free download from accellera.org)
- **"Analog and Mixed-Signal Modeling Using SystemC-AMS"** by Deantoni et al. — Detailed textbook treatment
- **Doulos SystemC AMS Tutorial** — Good companion with circuit-focused examples
- **Cadence Virtuoso AMS Designer** and **Mentor Graphics Platform Express** — Commercial tools supporting SystemC AMS

---

## A Note on Solvers and Numerical Methods

Unlike TLM where you write explicit function calls, AMS solvers work automatically behind the scenes. The LSF and ELN solvers use numerical integration methods (typically trapezoidal or Gear methods) to advance the continuous-time state. You do not need to implement these yourself, but you should be aware that:

- **Timestep size** affects both accuracy and solver stability
- **Very stiff systems** (large ratio between fastest and slowest time constants) may require very small timesteps
- **Algebraic loops** in TDF (feedback with no delay) will cause an elaboration error — always break loops with `set_delay()`
- The solver can be configured via `sca_core::sca_parameter` settings in some implementations

Understanding these limitations will help you debug unexpected simulation results.

---

*This tutorial series is designed to be studied sequentially within each section, but Sections 2, 3, and 4 (TDF, LSF, ELN) can be explored in parallel once Section 1 is complete. Section 5 requires all three MoC sections. Section 6 requires Section 5. Good luck — SystemC AMS opens the door to true mixed-signal virtual prototyping.*