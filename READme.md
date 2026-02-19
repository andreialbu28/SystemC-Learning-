# 🔌 SystemC Learning

![SystemC](https://img.shields.io/badge/SystemC-2.3.x-blue?style=flat-square)
![C++](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B)
![License](https://img.shields.io/badge/license-MIT-green?style=flat-square)
![Status](https://img.shields.io/badge/status-active-brightgreen?style=flat-square)

> **A curated collection of thoroughly commented SystemC examples, designed to help students and engineers master hardware/software co-simulation from scratch.**

---

## 📖 What is SystemC?

**SystemC** is a C++ class library standardized under **IEEE Std. 1666-2023**, developed and maintained by the [Accellera Systems Initiative](https://www.accellera.org/). It extends C++ with hardware modeling constructs — such as modules, ports, signals, and an event-driven simulation kernel — enabling designers to describe and simulate complex digital systems at multiple levels of abstraction.

Unlike traditional HDLs (VHDL, Verilog), SystemC lives entirely in C++, meaning you get the full power of a modern programming language combined with hardware modeling semantics. It is widely used in industry for:

- **System-Level Modeling (SLM)** — describe the behavior of a system before committing to RTL
- **Architectural Exploration** — compare different hardware architectures early in the design cycle
- **Transaction-Level Modeling (TLM 2.0)** — fast, abstract communication between IP blocks
- **High-Level Synthesis (HLS)** — some subsets of SystemC can be synthesized directly to RTL
- **Functional Verification** — build testbenches and simulate hardware designs
- **Software Development** — develop embedded software before the actual hardware is ready

---

## 🎯 Purpose of this Repository

This repository was created as a **learning companion** for anyone approaching SystemC for the first time — whether you are a student in a computer engineering or electronic engineering course, or a professional developer making the jump from C++ or Verilog to SystemC.

Every example in this repository follows a consistent philosophy:

- **Heavily commented code** — each line or block is explained inline so you can read the example like a tutorial
- **Incremental complexity** — examples are organized from basic building blocks to more advanced patterns
- **Standalone files** — each example can be compiled and run independently, without dependencies on the others
- **Real-world relevance** — patterns shown reflect actual usage in embedded systems and hardware design workflows

---

## 📁 Repository Structure

```
SystemC-Learning/
│
├── examples/           # Core learning material — commented SystemC examples
│                       # Each subdirectory covers a specific concept or algorithm
│
├── docs/               # Supplementary documentation, theory notes, and diagrams
│                       # Includes concept explanations and references to the IEEE standard
│
├── config/             # Build system configuration files
│                       # Environment and compiler settings for different platforms
│
├── scripts/            # Shell scripts for compiling and running examples
│                       # Automates the build process so you can focus on learning
│
├── external/           # Third-party dependencies (e.g., SystemC library stubs or headers)
│
├── mkdocs.yml          # Configuration for MkDocs — used to generate the docs website
├── .gitignore
└── README.md
```

---

## ⚙️ Prerequisites

Before cloning and running the examples, make sure you have the following tools installed on your system.

### Compiler

You need a C++ compiler with **C++17 support** or later:

```bash
# Check GCC version (should be 7+ for C++17)
g++ --version

# Or Clang
clang++ --version
```

### SystemC Library

Download and install SystemC from the official Accellera website:

🔗 https://www.accellera.org/downloads/standards/systemc

```bash
# Extract the archive
tar -xzf systemc-2.3.3.tar.gz

# Create build directory and configure
cd systemc-2.3.3
mkdir objdir && cd objdir
../configure --prefix=/usr/local/systemc-2.3.3

# Build and install
make -j$(nproc)
sudo make install
```

> **Note:** On macOS (Catalina or later), you may need to set the SDK root:
> ```bash
> export SDKROOT=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
> ```

### CMake (optional but recommended)

```bash
# Ubuntu/Debian
sudo apt install cmake

# macOS (via Homebrew)
brew install cmake
```

---

## 🚀 Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/andreialbu28/SystemC-Learning-.git
cd SystemC-Learning-
```

### 2. Compile a single example manually

```bash
g++ -std=c++17 \
    -I/usr/local/systemc-2.3.3/include \
    -L/usr/local/systemc-2.3.3/lib-linux64 \
    -Wl,-rpath=/usr/local/systemc-2.3.3/lib-linux64 \
    -lsystemc -lm \
    examples/your_example.cpp \
    -o your_example

./your_example
```

### 3. Use the provided build scripts

```bash
# Make scripts executable
chmod +x scripts/*.sh

# Run a specific example via script
./scripts/build.sh examples/your_example
```

---

## 🧠 Key SystemC Concepts Covered

The examples in this repository progressively introduce the following SystemC concepts:

| Concept | Description |
|---|---|
| `SC_MODULE` | Basic building block — equivalent to a hardware module |
| `SC_CTOR` | Module constructor — defines processes and sensitivity lists |
| `sc_in / sc_out / sc_inout` | Input, output, and bidirectional ports |
| `sc_signal` | Wire-like data carrier connecting modules |
| `SC_METHOD` | Process sensitive to signal changes (combinational logic) |
| `SC_THREAD` | Concurrent process that can be suspended with `wait()` |
| `SC_CTHREAD` | Clocked thread — sensitive to a single clock edge |
| `sc_clock` | Built-in clock generator |
| `sc_time` | Time values for delays and simulation control |
| `sc_start()` | Launch the SystemC simulation kernel |
| `sc_main()` | Entry point — analogous to `main()` in standard C++ |

---

## 📚 Documentation

Full documentation is available locally via **MkDocs**. To build and serve it:

```bash
# Install MkDocs
pip install mkdocs

# Serve the docs locally
mkdocs serve

# Open in browser
open http://127.0.0.1:8000
```

The `docs/` folder contains theory notes, diagrams, and extended explanations for each example category.

---

## 🤝 Contributing

Contributions are very welcome! If you want to add a new example, improve an existing comment, or fix a bug:

1. Fork the repository
2. Create a new branch: `git checkout -b feature/my-new-example`
3. Add your commented example under `examples/`
4. Open a Pull Request with a clear description of what you added

Please make sure your code compiles cleanly and that your comments are clear enough for a beginner to follow.

---

## 🔗 Useful Resources

- 📘 [Accellera SystemC Standard](https://www.accellera.org/downloads/standards/systemc)
- 📗 [IEEE Std. 1666-2023 — SystemC LRM](https://ieeexplore.ieee.org/document/10246125)
- 🌐 [Learn SystemC with Examples](https://learnsystemc.com/)
- 📙 *SystemC: From the Ground Up* — D. Black, J. Donovan, B. Bunton, A. Keist

---

## 📄 License

This project is released under the **MIT License**. See the `LICENSE` file for details.

---
