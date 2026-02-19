---
title: FMI & Standardization
description: Bridging SystemC with the Functional Mock-up Interface standard for cross-domain co-simulation.
tags:
  - FMI
  - co-simulation
  - standardization
  - SDV
---

# 🌐 FMI & Standardization

Modern embedded systems, especially in the automotive sector, are inherently **multi-domain**. A single vehicle Electronic Control Unit (ECU) may involve mechanical actuators, thermal models, power electronics, digital control logic, and embedded software all interacting at once. Simulating all of these together, with a single tool, is practically impossible.

The answer is **co-simulation**: each domain is modeled in the tool best suited for it, and a standardized interface coordinates the exchange of data and time between them at runtime.

This section documents the research efforts at **Politecnico di Torino** (in collaboration with **Dumarey Group**) aimed at bridging the SystemC ecosystem (at both the RTL and TLM abstraction levels) with the **Functional Mock-up Interface (FMI)** standard, enabling SystemC models to participate as first-class citizens in heterogeneous co-simulation workflows.

---

## 📚 Contents

<div class="grid cards" markdown>

-   :material-information-outline: **[What is FMI?](fmi-overview.md)**

    ---
    An introduction to the FMI standard, FMUs, the three interaction modes, and why FMI has become the dominant co-simulation standard in the automotive and cyber-physical systems industries.

-   :material-chip: **[SystemC RTL as FMU](rtl-fmu-methodology.md)**

    ---
    Methodology for automatically wrapping SystemC RTL models as FMI-compliant FMUs, enabling cycle-accurate hardware simulation within FMI co-simulation environments.

    [:octicons-arrow-right-24: Pollo et al. (2025)](pollo2025.md)

-   :material-transfer: **[SystemC TLM as FMU](tlm-fmu-methodology.md)**

    ---
    Non-intrusive, open-source toolchain for encapsulating SystemC TLM components as FMI 3.0 Co-Simulation FMUs, addressing time synchronization and data exchange.

    [:octicons-arrow-right-24: Albu et al. (2025)](albu2025.md)

-   :material-math-compass: **[Co-simulation with Simulink](co-simulation-simulink.md)**

    ---
    Step-by-step walkthrough of a cross-tool co-simulation between a SystemC-TLM-based FMU and a Simulink-based FMU, validating full FMI 3.0 standard compliance.

</div>

---

## 🗺️ Architecture

The diagram below shows how the two methodologies documented in this section fit together into a unified SystemC-to-FMI framework:

```
            ┌──────────────────────────────────────────────────┐
            │                SystemC Ecosystem                 │
            │                                                  │
            │   ┌───────────────┐     ┌──────────────────────┐ │
            │   │  SystemC RTL  │     │   SystemC TLM        │ │
            │   │  (SC_MODULE,  │     │  (b_transport,       │ │
            │   │   SC_CTHREAD) │     │   tlm_generic_payload│ │
            │   └──────┬────────┘     └──────────┬───────────┘ │
            │          │  Pollo et al.           │ Albu et al. │
            │          ▼                         ▼             │
            │   ┌─────────────────────────────────────────────┐│
            │   │         Automated FMI Wrapper Generator     ││
            │   │   (3-phase: Analysis → Wrapping → Validate) ││
            │   └────────────────────────┬────────────────────┘│
            └────────────────────────────┼─────────────────────┘
                                         │
                                         ▼
                        ┌─────────────────────────────────┐
                        │      FMI 3.0 Co-Simulation FMU  │
                        │  ┌───────────────────────────┐  │
                        │  │  modelDescription.xml     │  │
                        │  │  FMI C-API (fmi3.h)       │  │
                        │  │  SystemC kernel (embedded)│  │
                        │  └───────────────────────────┘  │
                        └────────────────┬────────────────┘
                                         │
                        ┌────────────────▼────────────────┐
                        │    FMI Master / Orchestrator    │
                        │  (e.g., FMPy, Simulink, Dymola) │
                        │                                 │
                        │  ┌──────────┐  ┌─────────────┐  │
                        │  │ SystemC  │  │  Simulink   │  │
                        │  │   FMU    │◄─►│    FMU     │  │
                        │  └──────────┘  └─────────────┘  │
                        └─────────────────────────────────┘
```

---

## 📄 Publications

| Paper | Authors | Venue | Links |
|---|---|---|---|
| *Integrating SystemC TLM into FMI 3.0 Co-Simulations with an Open-Source Approach* | Albu, Pollo, Burrello, Jahier Pagliari, Tesconi, Neri, Soldi, Autieri, Vinco | In press (2025) | [arXiv](https://arxiv.org/abs/2508.20223) · [Details](albu2025.md) |
| *Automatic integration of SystemC in the FMI standard for Software-defined Vehicle design* | Pollo, Albu, Burrello, Jahier Pagliari, Tesconi, Panaro, Soldi, Autieri, Vinco | FDL 2025 | [IEEE](https://ieeexplore.ieee.org/document/11165273) · [Details](pollo2025.md) |