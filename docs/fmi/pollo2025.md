---
title: "Paper: Pollo et al. (2025)"
description: Full summary of the paper on automatic SystemC RTL to FMI integration for Software-Defined Vehicle design.
tags:
  - FMI
  - RTL
  - SDV
  - publication
---

# 📄 Automatic Integration of SystemC in the FMI Standard for Software-Defined Vehicle Design (Pollo et al. 2025)

## Full Citation

> **Pollo, G., Albu, A. M., Burrello, A., Jahier Pagliari, D., Tesconi, C., Panaro, L., Soldi, D., Autieri, F., & Vinco, S.**
> *Automatic Integration of SystemC in the FMI Standard for Software-Defined Vehicle Design.*
> Forum on Specification & Design Languages (FDL 2025).
> [IEEE](https://ieeexplore.ieee.org/document/11165273) · [Details](pollo2025.md) |

**Authors:**
: Giovanni Pollo¹, Andrei Mihai Albu¹, Alessio Burrello¹, Daniele Jahier Pagliari¹, Cristian Tesconi², Loris Panaro², Dario Soldi², Fabio Autieri², Sara Vinco¹

**Affiliations:**
: ¹ Politecnico di Torino, Italy · ² Dumarey Group, Italy

**Keywords:**
: SystemC · FMI · FMU · Software-Defined Vehicle · RTL · Co-simulation · IP Protection

---

## Abstract

The recent advancements of the automotive sector demand robust co-simulation methodologies that enable early validation and seamless integration across hardware and software domains. However, the lack of standardized interfaces and the dominance of proprietary simulation platforms pose significant challenges to collaboration, scalability, and IP protection. This paper presents an approach for **automatically wrapping SystemC models using the Functional Mock-up Interface (FMI) standard**. This method combines the modeling accuracy and fast time-to-market of SystemC with the interoperability and encapsulation benefits of FMI, enabling secure and portable integration of embedded components into co-simulation workflows. The methodology is validated on real-world case studies, demonstrating its effectiveness with complex designs.

---

## Context and Motivation

The Software-Defined Vehicle paradigm is pushing automotive OEMs and Tier-1 suppliers to develop ECU hardware and software in parallel, long before physical prototypes exist. Virtual prototyping with SystemC RTL enables accurate simulation of hardware blocks — bus interfaces, interrupt controllers, memory subsystems — but these models have historically been isolated from the broader simulation ecosystem.

Key problems this paper addresses:

- **Proprietary lock-in**: most existing SystemC-to-FMI solutions depend on commercial tools (e.g., Synopsys Platform Architect), limiting accessibility
- **No interrupt support**: prior FMI/SystemC integrations do not handle interrupt-like early step termination
- **Manual effort**: existing approaches require hand-crafting the FMI wrapper, which is error-prone and does not scale
- **IP exposure**: sharing SystemC source code with partners is often not acceptable — FMU packaging provides binary-only distribution

---

## Methodology Summary

The paper proposes a fully automated three-phase pipeline (see [RTL Methodology page](rtl-fmu-methodology.md) for technical detail):

```
SystemC RTL Source
       │
       ▼  Phase 1: Design Analysis
       │  Parse SC_MODULE ports → identify FMI variables
       │
       ▼  Phase 2: FMI Wrapper Generation
       │  Generate modelDescription.xml + FMI C-API
       │  Map sc_in/sc_out → FMI inputs/outputs
       │  Implement fmi3DoStep → sc_start(step)
       │
       ▼  Phase 3: Compilation & Packaging
          Compile SystemC + wrapper → .so / .dll
          Package into .fmu archive
```

**Key design choices:**

- **Non-intrusive**: zero modifications to the original SystemC source
- **FMI 3.0**: takes advantage of binary variables and improved type support
- **Interrupt-aware**: the wrapper monitors internal SystemC events and propagates early-return signals through the FMI API
- **Open-source**: the full toolchain is publicly available

---

## Case Studies

The methodology was validated on **real-world industrial case studies** provided by Dumarey Group, covering a range of complexity and communication-computation tradeoffs:

| Case Study | Description | Complexity |
|---|---|---|
| **CS1** | Simple peripheral register block — read/write interface | Low |
| **CS2** | Multi-channel DMA controller with interrupt generation | Medium |
| **CS3** | Complex bus bridge with pipelined transactions and error handling | High |

For each case study, the generated FMU was validated for:

- Functional correctness: simulation output matches standalone SystemC execution
- FMI compliance: verified with the [FMI Compliance Checker](https://github.com/modelica/fmi-compliance-checker)
- Performance: overhead introduced by the FMI wrapper layer
- Portability: FMU loaded and run under both FMPy (Linux) and Simulink (Windows)

---

## Results and Contributions

!!! success "Key results"
    - The methodology generates fully FMI 3.0-compliant FMUs **automatically**, with no manual intervention beyond specifying the top-level module name
    - Generated FMUs produce **bit-identical** outputs compared to standalone SystemC simulation
    - Wrapper overhead is negligible for communication step sizes above 1 µs
    - Full **IP protection**: only the compiled binary and the XML interface are distributed — no source code

The paper makes the following original contributions:

1. The **first fully open-source** automated pipeline for wrapping SystemC RTL models as FMI 3.0 Co-Simulation FMUs
2. A solution for **interrupt-like early return** in the FMI `fmi3DoStep` function, driven by SystemC internal events
3. Validation on **industrial case studies** from the SDV domain, including cross-platform validation between Linux and Windows

---

## Relation to Albu et al.

This paper focuses on **RTL-level** SystemC models (signal-accurate, cycle-accurate). The companion paper by Albu et al. addresses the **TLM-level** (transaction-based, abstracting away signal timing). Together they provide a complete methodology covering both abstraction layers of the SystemC ecosystem:

| | Pollo et al. | Albu et al. |
|---|---|---|
| **Abstraction level** | RTL (`SC_MODULE`, `sc_in/out`) | TLM 2.0 (`b_transport`, generic payload) |
| **FMI data model** | Scalar/array port signals | Structured payload fields |
| **Primary use case** | Cycle-accurate HW simulation in co-sim | ESL virtual prototype in co-sim |
| **Time model** | Synchronous step + early return for interrupts | Step-based + anticipation for TLM responses |

---

## Links

- 📄 [IEEE](https://ieeexplore.ieee.org/document/11165273)
- 🔧 [RTL Methodology details](rtl-fmu-methodology.md)
- 🔁 [Companion paper — Albu et al.](albu2025.md)