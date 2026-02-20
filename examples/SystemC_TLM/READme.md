# SystemC TLM Tutorial — Complete Guide

Welcome to the SystemC Transaction-Level Modeling (TLM) tutorial series. This guide is written for someone who has completed the SystemC RTL fundamentals series (processes, events, signals, ports) and is ready to move up one abstraction level. If you are brand new to SystemC, start with the RTL series first — TLM builds directly on those concepts.

---

## What Is TLM and Why Does It Matter?

When you model a hardware system at the **Register-Transfer Level (RTL)**, every signal transition is simulated cycle by cycle. This is accurate but extremely slow for large systems — simulating a full SoC at RTL can take days.

**Transaction-Level Modeling** solves this by raising the abstraction level. Instead of toggling individual signals, TLM models communication as **function calls** called transactions. A CPU reading from memory is no longer thousands of signal transitions — it is one function call that passes a payload (address, data, command) from an initiator to a target and returns a response.

The result: TLM simulations run **100x to 10,000x faster** than RTL, making it practical to:

- Explore SoC architectures before RTL is written
- Develop and test embedded software on a virtual platform
- Run performance analysis and bottleneck detection
- Validate system-level behaviour and memory maps

TLM-2.0 is the industry standard, defined by the Open SystemC Initiative (OSCI) and ratified as part of IEEE 1666-2011. It is the lingua franca of virtual prototyping.

---

## Prerequisites

Before starting this series you should be comfortable with:

| Topic | Covered in |
|---|---|
| SystemC modules, processes (`SC_THREAD`, `SC_METHOD`) | Examples 01–06 |
| Events, sensitivity, delta cycles | Examples 07–10 |
| Primitive channels (`sc_mutex`, `sc_fifo`) | Examples 11–13 |
| `sc_signal`, ports (`sc_in`, `sc_out`) | Example 14 |
| C++ classes, inheritance, templates | General C++ knowledge |

---

## The TLM-2.0 Mental Model

Before diving into code, it helps to have the right mental picture.

```
  ┌─────────────┐        transaction        ┌─────────────┐
  │  INITIATOR  │ ───────────────────────► │   TARGET    │
  │  (e.g. CPU) │ ◄─────────────────────── │ (e.g. Memory│
  └─────────────┘        response           └─────────────┘
        │                                         │
   initiator_socket                         target_socket
```

- The **initiator** is the active side — it creates a transaction and calls the transport function.
- The **target** is the passive side — it receives the transaction, processes it, and fills in the response.
- The **socket** is the connection point on each side — it replaces individual ports and signals with a single standardised interface.
- The **generic payload** is the transaction object — it carries the command (READ/WRITE), address, data pointer, and response status.

This is the pattern every TLM design is built on, from the simplest memory model to a full virtual platform with CPU, bus fabric, peripherals, and DMA.

---

## Tutorial Structure and Timeline

The tutorial is divided into four sections, each building on the previous. Work through them in order.

---

### Section 1 — `15_TLM_Basics` — The Core Transaction Model

**Goal:** Understand the fundamental TLM-2.0 building blocks from scratch.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_blocking_transport` | First socket and b_transport | How to write a minimal initiator and target, connect them via sockets, and call `b_transport` to transfer data |
| `02_generic_payload` | Anatomy of the payload | The full structure of `tlm_generic_payload`: command, address, data pointer, length, byte enables, response status |
| `03_transport_delay` | Modeling timing | How TLM represents time using an `sc_time` delay argument, and the difference between annotated delay and real simulation time advance |

**Key concepts introduced:**

`tlm_initiator_socket`, `tlm_target_socket`, `b_transport`, `tlm_generic_payload`, `set_command()`, `set_address()`, `set_data_ptr()`, `set_data_length()`, `set_response_status()`, `TLM_READ_COMMAND`, `TLM_WRITE_COMMAND`, `TLM_OK_RESPONSE`

**By the end of Section 1** you will be able to write a complete, working initiator-target pair that transfers data through a socket and understand exactly what information travels in each direction.

---

### Section 2 — `16_TLM_Sockets` — Connecting Modules

**Goal:** Move beyond a single initiator-target pair to understand how sockets work in realistic multi-module designs.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_simple_socket` | `tlm_utils` convenience sockets | How `tlm_utils::simple_initiator_socket` and `simple_target_socket` eliminate boilerplate by using callbacks instead of interface inheritance |
| `02_multi_passthrough_socket` | One initiator, many targets | How a single initiator socket can connect to multiple targets using `multi_passthrough_initiator_socket` and tagged callbacks that identify the source |
| `03_hierarchical_binding` | Sockets through module hierarchy | How to pass sockets through a module hierarchy using port-to-port binding, and how `sc_export` exposes a child's socket as a parent's interface |

**Key concepts introduced:**

`tlm_utils::simple_initiator_socket`, `tlm_utils::simple_target_socket`, callback registration (`register_b_transport`), `multi_passthrough_initiator_socket`, tagged transport (`tagged_b_transport`), hierarchical socket binding, `sc_export`

**By the end of Section 2** you will understand how to wire multi-module designs with sockets and how to build reusable module components with clean TLM interfaces.

---

### Section 3 — `17_TLM_Coding_Styles` — Accuracy vs. Speed

**Goal:** Understand the two official TLM-2.0 coding styles and when to use each.

TLM-2.0 defines two interoperability standards for transport:

| Style | Name | Description |
|---|---|---|
| **LT** | Loosely Timed | Uses `b_transport` with annotated delay. Fast, simple, approximate timing. |
| **AT** | Approximately Timed | Uses `nb_transport_fw/bw` with phase-based handshake. Slower, models bus pipeline accurately. |

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_loosely_timed` | LT coding style | How to use `b_transport` with a time annotation for fast architectural simulation, and what "temporal decoupling" means |
| `02_approximately_timed` | AT coding style | How the non-blocking transport protocol works with BEGIN_REQ / END_REQ / BEGIN_RESP / END_RESP phases and backward path callbacks |
| `03_quantum_keeper` | Temporal decoupling and quantum | How `tlm_utils::tlm_quantumkeeper` allows an initiator to run ahead of the simulator clock by a quantum, and how to configure the global quantum for speed vs. accuracy |

**Key concepts introduced:**

`b_transport` with delay annotation, temporal decoupling, `nb_transport_fw`, `nb_transport_bw`, `tlm_phase` (`BEGIN_REQ`, `END_REQ`, `BEGIN_RESP`, `END_RESP`), `tlm_sync_enum` (`TLM_ACCEPTED`, `TLM_UPDATED`, `TLM_COMPLETED`), `tlm_utils::tlm_quantumkeeper`, `set_global_quantum`

**By the end of Section 3** you will understand the fundamental trade-off between simulation speed and timing accuracy, and be able to choose and implement the right coding style for a given project.

---

### Section 4 — `18_TLM_Memory_Model` — Building a Real System

**Goal:** Apply everything learned to build a complete virtual platform with a memory and an interconnect.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_simple_memory` | Memory target | How to implement a realistic memory target with address decoding, byte enables, and burst transfer support |
| `02_interconnect` | Bus / router | How to build a router module that sits between initiators and targets, decodes addresses, and routes transactions to the correct target |
| `03_initiator_thread` | CPU-like initiator | How to write a realistic initiator thread that generates sequences of READ and WRITE transactions, checks response status, and handles errors |

**Key concepts introduced:**

Address map, address decoding, byte enables (`set_byte_enable_ptr`), burst transfers (`set_streaming_width`), router/interconnect pattern, response status checking, `TLM_ADDRESS_ERROR_RESPONSE`, `TLM_COMMAND_ERROR_RESPONSE`

**By the end of Section 4** you will have built a small but complete virtual platform — CPU initiator → bus router → memory targets — that can serve as a skeleton for real SoC modeling.

---

### Section 5 — `19_TLM_Advanced` — Performance and Extensibility

**Goal:** Learn the advanced TLM-2.0 features that make virtual platforms fast enough for running real software.

| Sub-example | Title | What you will learn |
|---|---|---|
| `01_dmi` | Direct Memory Interface | How DMI allows an initiator to bypass the transport layer and access target memory directly as a C++ pointer, dramatically accelerating instruction fetch and bulk data transfer |
| `02_debug_transport` | Debug transport | How `transport_dbg` provides non-intrusive read/write access to target memory without affecting simulation state or consuming simulation time — used by debuggers and analysis tools |
| `03_extensions` | Custom payload extensions | How to attach custom data to `tlm_generic_payload` using the extension mechanism, and how targets retrieve and use those extensions |

**Key concepts introduced:**

`get_direct_mem_ptr`, `tlm_dmi`, `invalidate_direct_mem_ptr`, DMI region (start address, end address, pointer, latency), `transport_dbg`, `tlm_extension<T>`, `set_extension`, `get_extension`, `free_all_extensions`

**By the end of Section 5** you will understand the complete TLM-2.0 feature set and be equipped to build high-performance virtual platforms capable of booting real embedded software.

---

## Learning Path Summary

```
Section 1 — Basics          Section 2 — Sockets
┌───────────────────┐        ┌───────────────────┐
│ 01 b_transport    │        │ 01 simple_socket   │
│ 02 payload        │  ────► │ 02 multi_socket    │
│ 03 delay          │        │ 03 hierarchical    │
└───────────────────┘        └───────────────────┘
          │                            │
          └──────────────┬─────────────┘
                         ▼
          Section 3 — Coding Styles
          ┌───────────────────────────┐
          │ 01 loosely timed (LT)     │
          │ 02 approximately timed(AT)│
          │ 03 quantum keeper         │
          └───────────────────────────┘
                         │
                         ▼
          Section 4 — Memory Model
          ┌───────────────────────────┐
          │ 01 simple memory          │
          │ 02 interconnect/router    │
          │ 03 CPU initiator          │
          └───────────────────────────┘
                         │
                         ▼
          Section 5 — Advanced
          ┌───────────────────────────┐
          │ 01 DMI                    │
          │ 02 debug transport        │
          │ 03 extensions             │
          └───────────────────────────┘
```

---

## Estimated Study Time

| Section | Sub-examples | Estimated time |
|---|---|---|
| 15 — Basics | 3 | 3–4 hours |
| 16 — Sockets | 3 | 2–3 hours |
| 17 — Coding Styles | 3 | 4–6 hours |
| 18 — Memory Model | 3 | 4–5 hours |
| 19 — Advanced | 3 | 3–4 hours |
| **Total** | **15** | **16–22 hours** |

These estimates assume you read the README, study the code, compile and run it, and experiment with small modifications. Rushing through without running the examples will reduce understanding significantly.

---

## How to Read Each Example

Every sub-example in this series follows the same structure:

1. **Read the README first** — understand what the example is teaching before looking at the code
2. **Trace the transaction lifecycle** — follow a single transaction from initiator through socket to target and back
3. **Run the example and match the output** to the expected output in the README
4. **Modify and experiment** — change a timing value, swap a READ for a WRITE, add a second target
5. **Move on only when the output makes sense** — TLM concepts build on each other; gaps early on cause confusion later

---

## Key TLM-2.0 Reference — At a Glance

### The Generic Payload — Fields

| Field | Setter | Getter | Description |
|---|---|---|---|
| Command | `set_command(cmd)` | `get_command()` | `TLM_READ_COMMAND` or `TLM_WRITE_COMMAND` |
| Address | `set_address(addr)` | `get_address()` | Target byte address |
| Data pointer | `set_data_ptr(ptr)` | `get_data_ptr()` | Pointer to data buffer |
| Data length | `set_data_length(n)` | `get_data_length()` | Number of bytes |
| Byte enable | `set_byte_enable_ptr(ptr)` | `get_byte_enable_ptr()` | Which bytes are active |
| Streaming width | `set_streaming_width(n)` | `get_streaming_width()` | Burst wrap width |
| Response | `set_response_status(s)` | `get_response_status()` | Result code |

### Transport Interfaces

| Function | Style | Direction | Description |
|---|---|---|---|
| `b_transport(payload, delay)` | LT | Forward | Blocking call — completes the full transaction |
| `nb_transport_fw(payload, phase, delay)` | AT | Forward | Non-blocking forward path (initiator → target) |
| `nb_transport_bw(payload, phase, delay)` | AT | Backward | Non-blocking backward path (target → initiator) |
| `get_direct_mem_ptr(payload, dmi)` | Both | Forward | Request DMI access region |
| `invalidate_direct_mem_ptr(start, end)` | Both | Backward | Revoke DMI region |
| `transport_dbg(payload)` | Both | Forward | Non-intrusive debug access |

### Response Status Codes

| Code | Meaning |
|---|---|
| `TLM_OK_RESPONSE` | Transaction completed successfully |
| `TLM_ADDRESS_ERROR_RESPONSE` | Address not mapped |
| `TLM_COMMAND_ERROR_RESPONSE` | Command not supported |
| `TLM_BURST_ERROR_RESPONSE` | Burst parameters invalid |
| `TLM_GENERIC_ERROR_RESPONSE` | Other error |

---

## Further Reading

Once you complete this tutorial series, the recommended next resources are:

- **IEEE Std 1666-2011** — The official SystemC and TLM-2.0 standard (authoritative reference)
- **"A Practical Guide to Adopting the Universal Verification Methodology"** — For UVM/SystemVerilog integration
- **Doulos SystemC TLM-2.0 Tutorial** — Good companion reference with additional examples
- **ARM Fast Models documentation** — Real-world TLM virtual platform example
- **QEMU + SystemC bridge examples** — For connecting TLM platforms to real software stacks

---

## A Note on Tooling

All examples in this series are written against the **SystemC 3.x** library with the TLM-2.0 headers included. To compile:

```bash
# Build using the provided test.sh script in each sub-example directory
./test.sh all   # compile
./test.sh run   # run
./test.sh clean # clean build artifacts
```

Ensure your `SYSTEMC_HOME` or equivalent path variable points to your SystemC installation. The TLM headers are included in the standard SystemC distribution under `include/tlm/`.

---

*This tutorial series was designed to be read sequentially. Each example is self-contained but assumes knowledge of all preceding examples. Good luck — TLM is one of the most powerful tools in the hardware engineer's and verification engineer's toolkit.*