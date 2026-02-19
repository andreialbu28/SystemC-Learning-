---
title: What is FMI?
description: An introduction to the Functional Mock-up Interface standard, FMUs, and why it matters for SystemC.
tags:
  - FMI
  - co-simulation
  - FMU
---

# What is FMI?

The **Functional Mock-up Interface (FMI)** is an open, tool-independent standard that defines a unified C-based API for the exchange and co-simulation of dynamic models across heterogeneous simulation environments. Originally developed by Daimler AG as part of the MODELISAR ITEA2 project and first published in 2010, FMI is today maintained by the **Modelica Association** and has become the *de facto* co-simulation standard in the automotive, aerospace, and cyber-physical systems industries.

!!! info "Current version"
    The latest release is **FMI 3.0** (2022), which introduced support for binary variables, array-typed variables, clocked variables for event-driven systems, and improved terminal/bus support — features that are particularly relevant for interfacing with hardware simulation environments such as SystemC.

---

## The Functional Mock-up Unit (FMU)

An FMI-compliant model is packaged as a **Functional Mock-up Unit (FMU)** — a portable, self-contained archive (`.fmu`, actually a ZIP file) that encapsulates everything needed to run the model in any FMI-compatible environment:

```
my_model.fmu
├── modelDescription.xml    # Interface declaration (variables, causality, types)
├── binaries/
│   ├── linux64/
│   │   └── my_model.so    # Shared library implementing the FMI C-API
│   └── win64/
│       └── my_model.dll
├── resources/              # Model-specific data files, parameters
└── documentation/          # Optional: model documentation
```

The `modelDescription.xml` is the contract between the FMU and the outside world. It declares every variable the model exposes — its name, type, causality (`input`, `output`, `parameter`, `local`), and variability — without revealing any internal implementation details. This makes FMI an effective mechanism for **IP protection**: the model logic is compiled into a binary, while only the interface is visible.

---

## Three Interaction Modes

FMI defines three main modes of interaction between an FMU and its hosting environment:

1. **Co-Simulation (CS)**"**

    The FMU contains its own internal solver. The FMI master simply steps forward in time by calling `fmi3DoStep()`, passing the current time and step size. The FMU handles all internal time integration autonomously.

    This is the mode used in both **Albu et al.** and **Pollo et al.** — the SystemC simulation kernel runs *inside* the FMU, and the FMI master coordinates time with the SystemC time.

    ```
    Master:  fmi3DoStep(t=0.0, h=0.001)
               ─────────────────────────►  FMU: runs SystemC for 1ms
             fmi3GetFloat64(output)         
               ◄─────────────────────────  FMU: returns computed output
    ```

2. **Model Exchange (ME)**

    The FMU exposes its mathematical equations but relies on the FMI master's numerical solver to integrate them over time. Suited for continuous-time physical models (ODEs/DAEs). Not applicable to SystemC's event-driven simulation model.

3. **Scheduled Execution (SE)**

    Introduced in FMI 3.0. The FMU exposes individually schedulable model partitions, suitable for real-time and multi-rate systems. Particularly relevant for future integration with SystemC AMS TDF models, which have explicit rate semantics.

---

## The FMI Master (Orchestrator)

The **FMI master** is the simulation environment that imports one or more FMUs and orchestrates their execution. It is responsible for:

- Stepping each FMU forward in time (calling `fmi3DoStep`)
- Routing output variables from one FMU to input variables of another
- Managing the global simulation time and step size
- Handling event detection and rollback (in ME and SE modes)

Common FMI masters include:

| Tool | Type | Notes |
|---|---|---|
| **FMPy** | Open-source Python | Excellent for scripting and testing |
| **Simulink** (MathWorks) | Commercial | Widely used in automotive industry |
| **Dymola** (Dassault) | Commercial | Native Modelica + FMI support |
| **OpenModelica** + OMSimulator | Open-source | Academic standard |
| **INTO-CPS** | Open-source | Research-oriented multi-model co-simulation |

---

## Why FMI Matters for SystemC

SystemC has long excelled at hardware/software co-simulation within its own ecosystem but it has historically been a closed island. A virtual prototype built in SystemC TLM cannot directly exchange data with a thermal model in Modelica, a control system in Simulink, or a mechanical model in Adams — without custom, brittle glue code.

FMI solves this problem. By wrapping a SystemC model as an FMU:

- It becomes **tool-agnostic**: any FMI-compatible master can run it
- It offers **IP protection**: the source code is compiled into a binary
- It enables **multi-domain co-simulation**: mechanical, thermal, electrical, and digital models can all participate in the same simulation
- It supports the **Software-Defined Vehicle (SDV)** design flow, where ECU virtual prototypes must co-simulate with vehicle-level models from day one

---

## FMI 3.0 Key Improvements Relevant to SystemC

FMI 3.0 introduced several features that make the SystemC–FMI integration cleaner and more expressive:

| Feature | Relevance to SystemC |
|---|---|
| **Binary variables** | Can carry raw byte buffers — natural fit for TLM payload data |
| **Array variables** | Eliminates the need to flatten data structures into scalar lists |
| **Clocked variables** | Maps well to SystemC's event-driven and clocked process semantics |
| **Terminal & bus definitions** | Enables structured, typed communication between FMUs — analogous to TLM sockets |
| **Improved logging** | Better debugging during co-simulation of complex heterogeneous systems |

---

## Further Reading

- 📘 [FMI Standard website](https://fmi-standard.org/)
- 📗 [FMI 3.0 Specification (PDF)](https://svn.modelica.org/fmi/branches/public/specifications/v3.0/FMI-Spec-3.0.pdf)
- 🔗 [FMPy — Python FMI library](https://github.com/CATIA-Systems/FMPy)
- 📄 [Albu et al. — SystemC TLM + FMI 3.0](https://arxiv.org/abs/2508.20223)
- 📄 [Pollo et al. — SystemC RTL + FMI for SDV](https://ieeexplore.ieee.org/document/11165273) 