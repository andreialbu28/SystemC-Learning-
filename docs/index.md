# 🔌 SystemC Learning ( Work in Progress )

![SystemC](https://img.shields.io/badge/SystemC-IEEE_1666--2023-blue?style=flat-square)
![TLM](https://img.shields.io/badge/TLM-2.0-orange?style=flat-square)
![AMS](https://img.shields.io/badge/AMS-IEEE_1666.1--2016-purple?style=flat-square)
![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)
![Status](https://img.shields.io/badge/status-active-brightgreen?style=flat-square)

> **A curated collection of thoroughly commented SystemC examples, designed to help students and engineers master hardware/software co-simulation from scratch — covering SystemC, TLM, and AMS.**

---

## 📖 What is SystemC?

**SystemC** is a C++ class library standardized under **IEEE Std. 1666-2023**, developed and maintained by the [Accellera Systems Initiative](https://www.accellera.org/). It extends C++ with hardware modeling constructs — such as modules, ports, signals, and an event-driven simulation kernel — enabling designers to describe and simulate complex digital systems at multiple levels of abstraction.

Unlike traditional Hardware Description Languages (HDLs) such as VHDL and Verilog, SystemC lives entirely in C++, meaning you get the full expressiveness and tooling of a modern programming language combined with hardware modeling semantics. It is widely used in industry for a broad range of tasks:

- **System-Level Modeling (SLM)** — describe the behavior of a system before committing to RTL
- **Architectural Exploration** — compare different hardware architectures early in the design cycle
- **Transaction-Level Modeling (TLM 2.0)** — fast, abstract communication modeling between IP blocks
- **Analog/Mixed-Signal Modeling (AMS)** — simulate continuous-time and mixed-domain systems
- **High-Level Synthesis (HLS)** — some subsets of SystemC can be synthesized directly to RTL
- **Functional Verification** — build testbenches and simulate hardware designs in software
- **Software Development** — develop and test embedded software before physical hardware is ready

---

## 🔁 SystemC TLM — Transaction-Level Modeling

**SystemC TLM 2.0** (Transaction-Level Modeling) is a modeling methodology and library built on top of SystemC that raises the level of abstraction from signal-level communication to *transaction-based* communication. Instead of modeling every clock cycle and wire toggle, TLM allows you to describe the *intent* of a data transfer as a single transaction object — dramatically improving simulation speed and making it possible to simulate entire Systems-on-Chip (SoC) at millions of transactions per second.

### Why TLM?

At the RTL level, simulation of a complex SoC can take hours or days. TLM makes it possible to boot operating systems and run application software on a virtual prototype in minutes, enabling software development to begin long before tape-out. This is the fundamental value proposition of **Electronic System-Level (ESL) design**.

### TLM 2.0 Core Concepts

| Concept | Description |
|---|---|
| **Initiator / Target** | Two roles in a TLM transaction: the initiator sends a request, the target responds |
| **tlm_generic_payload** | A standardized transaction object carrying address, data, command, and response status |
| **tlm_initiator_socket / tlm_target_socket** | Standardized port interfaces that connect initiators and targets |
| **Blocking transport (`b_transport`)** | The initiator calls a function and blocks until the transaction completes — simple and easy to use |
| **Non-blocking transport (`nb_transport`)** | Asynchronous communication with forward/backward phases — suitable for pipelined protocols |
| **Direct Memory Interface (DMI)** | A shortcut that allows a target to grant the initiator direct memory access, bypassing the socket for maximum simulation speed |
| **Loosely Timed (LT) coding style** | Focuses on functional correctness; time is approximate — ideal for early software development |
| **Approximately Timed (AT) coding style** | Adds timing detail for pipeline and latency modeling — better for hardware/software co-validation |

### TLM in This Repository

The examples in `examples/` progressively introduce TLM concepts: from simple blocking transport between two modules, to multi-initiator/multi-target topologies, router models, and interrupt handling via TLM sockets.

---

## 〰️ SystemC AMS — Analog/Mixed-Signal Extensions

**SystemC AMS** (standardized as **IEEE Std. 1666.1-2016**) extends the base SystemC framework with the ability to model and simulate *analog, mixed-signal, and RF* systems at a high level of abstraction. It was the first system-level modeling language to describe analog/mixed-signal behavior as a natural extension of an existing digital ESL methodology.

### Why AMS?

Modern embedded systems are deeply mixed-signal: sensors, ADCs, PLLs, RF transceivers, power converters, and actuators all interact with digital processing cores and software stacks. Simulating all of these at the transistor level (SPICE) is prohibitively slow for system-level exploration. SystemC AMS provides a middle ground: fast, abstract simulation of continuous-time behavior that can be co-simulated alongside the digital and software components of the same virtual prototype — enabling a seamless integration of HW/SW architectures in SystemC, TLM, embedded software in C, and analog functionality modeled with AMS extensions.

### Models of Computation

SystemC AMS defines three complementary models of computation, each suited to a different modeling style:

| Model of Computation | Description | Typical Use Case |
|---|---|---|
| **Timed Data Flow (TDF)** | Discrete-time dataflow with smart temporal abstraction; much faster than event-driven simulation | Signal processing chains, ADC/DAC behavioral models, communication protocols |
| **Linear Signal Flow (LSF)** | Continuous-time modeling using a linear DAE solver; describes signal relationships as equations | Control loops, filter models, behavioral analog blocks |
| **Electrical Linear Networks (ELN)** | Models analog circuits as networks of lumped electrical elements (R, C, L, voltage/current sources) | RC networks, power supply models, interface and interconnect modeling |

These three models can be freely mixed in the same simulation and interface naturally with standard SystemC digital processes and TLM sockets — enabling complete, heterogeneous virtual prototypes in a single C++ compilation unit.

### AMS in This Repository

The `examples/` folder includes AMS examples demonstrating how to create TDF modules, connect AMS signals to digital SystemC modules, and model simple mixed-signal systems such as a sampled-data filter or a sensor interface with ADC behavior.

> **Prerequisites for AMS:** A proof-of-concept open-source implementation is available from [COSEDA Technologies](http://www.coseda-tech.com/) under the Apache 2.0 license. Commercial simulators such as **COSIDE** also provide full AMS support. The `external/` folder in this repository already bundles SystemC AMS 2.3 for convenience.

---

## 📦 Pre-installed Libraries in `external/`

This repository includes pre-compiled or source-ready versions of the SystemC libraries in the `external/` folder so you can get started without manually downloading and building them. The following versions are bundled:

| Library | Version | Standard | Notes |
|---|---|---|---|
| `systemc` | 2.3.3 | IEEE 1666-2011 | Core SystemC + TLM 2.0 included |
| `systemc` | 2.3.4 | IEEE 1666-2011 | Core SystemC + TLM 2.0 included |
| `systemc` | 3.0.2 | IEEE 1666-2023 | Core SystemC + TLM 2.0 included |
| `systemc-ams` | 2.3 | IEEE 1666.1-2016 | AMS extensions (TDF, LSF, ELN) |

These are sourced directly from the official [Accellera downloads](https://www.accellera.org/downloads/standards/systemc) and are provided here for reproducibility. If a newer version is released, you can replace the contents of `external/` with the updated distribution and rebuild using the provided scripts.

> **License note:** SystemC and SystemC AMS are distributed under the Apache 2.0 License. By using the libraries in `external/`, you agree to the terms of that license.

---

## 🌐 SystemC and FMI: Towards Cross-Domain Co-Simulation Standardization

As embedded systems grow in complexity — particularly in the automotive domain with the rise of **Software-Defined Vehicles (SDV)** — the need to co-simulate heterogeneous models from different engineering disciplines (mechanical, thermal, electrical, and software) has become critical. SystemC excels at hardware/software modeling, but its ecosystem has historically been isolated from other simulation tools and domain-specific standards.

The **Functional Mock-up Interface (FMI)** is an open industry standard for model exchange and co-simulation between heterogeneous simulation environments. An FMI-compliant model is packaged as a **Functional Mock-up Unit (FMU)** — a portable, encapsulated binary that can be plugged into any FMI-compatible simulation master, regardless of the originating tool. Since its first release in 2010, FMI has become the *de facto* co-simulation standard in the automotive and cyber-physical systems industries.

Bridging SystemC and FMI is an active and strategically important research area. Recent work from **Politecnico di Torino** (in collaboration with Dumarey Group) has addressed this challenge from two complementary angles, covering both the TLM and RTL abstraction levels of SystemC:

**Albu et al.** present a fully open-source methodology for integrating **SystemC TLM** models into **FMI 3.0** co-simulation workflows. Their approach encapsulates SystemC TLM components as FMI 3.0 Co-Simulation FMUs, addressing key technical challenges including time synchronization between the SystemC event-driven kernel and the FMI step-based runtime, and bidirectional data exchange through the TLM generic payload interface. The integration is non-intrusive — existing TLM models require no modification — and the toolchain is fully automated. Validation through industrial case studies, including a co-simulation with a Simulink-based FMU, demonstrates full FMI standard compliance and seamless cross-tool interoperability across heterogeneous simulation environments.

> **[Albu et al., 2025]** Albu, A. M., Pollo, G., Burrello, A., Jahier Pagliari, D., Tesconi, C., Neri, A., Soldi, D., Autieri, F., & Vinco, S. *Integrating SystemC TLM into FMI 3.0 Co-Simulations with an Open-Source Approach*. arXiv:2508.20223. https://arxiv.org/abs/2508.20223

**Pollo et al.** address the complementary problem of wrapping **SystemC RTL** models — where cycle-accurate timing and detailed signal semantics are essential — as FMUs for integration into automotive co-simulation workflows. Their automated three-step methodology generates a complete FMI wrapper from any SystemC design without requiring modifications to the source model, combining the modeling precision of RTL simulation with the interoperability and IP-protection benefits of FMI encapsulation. The approach is validated on real-world industrial case studies from the Software-Defined Vehicle domain and is fully open-source.

> **[Pollo et al., 2025]** Pollo, G., Albu, A. M., Burrello, A., Jahier Pagliari, D., Tesconi, C., Panaro, L., Soldi, D., Autieri, F., & Vinco, S. *Automatic Integration of SystemC in the FMI Standard for Software-Defined Vehicle Design*. Forum on Specification & Design Languages (FDL 2025). arXiv:2508.19665. https://arxiv.org/abs/2508.19665

Together, these two works establish a coherent, open-source framework for integrating the full SystemC ecosystem — from RTL to TLM — into FMI-based co-simulation environments, enabling seamless multi-domain simulation for next-generation cyber-physical systems.

---

## 🎯 Purpose of This Repository

This repository was created as a **learning companion** for anyone approaching SystemC for the first time — whether you are a student in a computer or electronic engineering course, or a developer making the jump from C++, Verilog, or VHDL into the SystemC ecosystem.

Every example follows a consistent philosophy:

- **Heavily commented code** — each construct is explained inline so you can read examples like tutorials
- **Incremental complexity** — from simple hello-world modules to TLM routers and AMS mixed-signal systems
- **Standalone files** — each example compiles and runs independently
- **Real-world relevance** — patterns reflect actual usage in embedded systems and hardware design workflows

---

## 📁 Repository Structure

```
SystemC-Learning/
│
├── examples/           # Core learning material — commented SystemC, TLM, and AMS examples
│                       # Organized from introductory to advanced topics
│
├── docs/               # Supplementary documentation, theory notes, and diagrams
│                       # Includes concept explanations and references to the IEEE standards
│
├── config/             # Build system configuration files
│                       # Environment and compiler settings for different platforms
│
├── scripts/            # Shell scripts for compiling and running examples
│                       # Automates the build process so you can focus on learning
│
├── external/           # Pre-bundled SystemC 2.3.3 and SystemC AMS 2.3 libraries
│                       # Ready to use — no manual installation required
│
├── mkdocs.yml          # Configuration for MkDocs — used to generate the docs website
├── .gitignore
└── README.md
```

---

## ⚙️ Prerequisites

### Compiler

You need a C++ compiler with **C++17 support** or later:

```bash
# Check GCC version (should be 7+ for C++17)
g++ --version

# Or Clang
clang++ --version
```

### Using the bundled libraries (recommended)

The `external/` folder already contains SystemC 2.3.3 and SystemC AMS 2.3. The build scripts in `scripts/` are pre-configured to point to these paths — no separate installation is required.

### Manual installation (optional)

If you prefer a system-wide installation:

```bash
tar -xzf systemc-2.3.3.tar.gz
cd systemc-2.3.3
mkdir objdir && cd objdir
../configure --prefix=/usr/local/systemc-2.3.3
make -j$(nproc)
sudo make install
```

> **macOS note:** You may need to export the SDK root first:
> ```bash
> export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
> ```

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/andreialbu28/SystemC-Learning-.git
cd SystemC-Learning-
```

### 2. Build and run an example using the provided scripts

```bash
chmod +x scripts/*.sh
./scripts/build.sh examples/your_example
```

### 3. Compile a SystemC example manually (using bundled libraries)

```bash
g++ -std=c++17 \
    -I./external/systemc-2.3.3/include \
    -L./external/systemc-2.3.3/lib-linux64 \
    -Wl,-rpath=./external/systemc-2.3.3/lib-linux64 \
    -lsystemc -lm \
    examples/your_example.cpp -o your_example

./your_example
```

### 4. Compile an AMS example (also link the AMS library)

```bash
g++ -std=c++17 \
    -I./external/systemc-2.3.3/include \
    -I./external/systemc-ams-2.3/include \
    -L./external/systemc-2.3.3/lib-linux64 \
    -L./external/systemc-ams-2.3/lib-linux64 \
    -lsystemc-ams -lsystemc -lm \
    examples/ams_example.cpp -o ams_example

./ams_example
```

---

## 🧠 Key Concepts Reference

### Core SystemC

| Construct | Description |
|---|---|
| `SC_MODULE` | Basic building block — equivalent to a hardware module |
| `SC_CTOR` | Module constructor — defines processes and sensitivity lists |
| `sc_in / sc_out / sc_inout` | Input, output, and bidirectional signal ports |
| `sc_signal` | Wire-like data carrier connecting modules |
| `SC_METHOD` | Combinational process, triggered by signal changes |
| `SC_THREAD` | Concurrent process that can be suspended with `wait()` |
| `SC_CTHREAD` | Clocked thread sensitive to a single clock edge |
| `sc_clock` | Built-in clock generator |
| `sc_time` | Time representation for delays and simulation control |
| `sc_start()` | Launches the simulation kernel |

### SystemC TLM 2.0

| Construct | Description |
|---|---|
| `tlm_initiator_socket` | Port on the initiator side of a transaction |
| `tlm_target_socket` | Port on the target side of a transaction |
| `tlm_generic_payload` | Standard transaction object: address, data, command |
| `b_transport()` | Blocking transport — simple, synchronous transaction |
| `nb_transport_fw/bw()` | Non-blocking transport — pipelined protocols |
| `get_direct_mem_ptr()` | DMI — direct memory pointer for zero-copy fast simulation |

### SystemC AMS

| Construct | Description |
|---|---|
| `SCA_TDF_MODULE` | A Timed Data Flow module for discrete-time signal processing |
| `sca_tdf::sca_in / sca_out` | AMS TDF ports for data flow signals |
| `set_timestep()` | Sets the processing timestep for a TDF cluster |
| `sca_lsf::sca_gain` | Linear Signal Flow gain primitive |
| `sca_eln::sca_r / sca_c / sca_l` | Electrical linear network R, C, L elements |
| `sca_ac_start()` | Launches a frequency-domain AC analysis |

---

## 📚 Documentation

Full documentation is generated from the `docs/` folder using **MkDocs**:

```bash
pip install mkdocs
mkdocs serve
# then open http://127.0.0.1:8000
```

---

## 🤝 Contributing

Contributions are very welcome! To add a new example, improve comments, or fix a bug:

1. Fork the repository
2. Create a branch: `git checkout -b feature/my-new-example`
3. Add your commented example under `examples/`
4. Open a Pull Request with a clear description

Please ensure your code compiles cleanly and that comments are accessible to a beginner.

---

## 🔗 Useful Resources

- 📘 [Accellera SystemC Downloads](https://www.accellera.org/downloads/standards/systemc)
- 📗 [IEEE Std. 1666-2023 — SystemC LRM](https://ieeexplore.ieee.org/document/10246125)
- 📙 [IEEE Std. 1666.1-2016 — SystemC AMS LRM](https://ieeexplore.ieee.org/document/7906935)
- 🌐 [systemc.org — Standards & Resources](https://systemc.org/resources/standards/)
- 🌐 [Learn SystemC with Examples](https://learnsystemc.com/)
- 📄 [Albu et al. — SystemC TLM + FMI 3.0 (arXiv:2508.20223)](https://arxiv.org/abs/2508.20223)
- 📄 [Pollo et al. — SystemC RTL + FMI for SDV (arXiv:2508.19665)](https://arxiv.org/abs/2508.19665)
- 📕 *SystemC: From the Ground Up* — D. Black, J. Donovan, B. Bunton, A. Keist
- 📗 *SystemC AMS User's Guide* — Accellera Systems Initiative (2020)

---

## 📄 License

This project is released under the **MIT License**. See the `LICENSE` file for details.
The bundled SystemC and SystemC AMS libraries in `external/` are distributed under the **Apache 2.0 License** by Accellera Systems Initiative.

---

<p align="center">
  Made with ❤️ for the SystemC learning community · Andrei - Politecnico di Torino
</p>