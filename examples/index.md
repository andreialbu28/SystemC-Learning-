# SystemC Tutorial Examples

This comprehensive tutorial collection covers all major SystemC concepts from basic to advanced. Each example is self-contained with proper header/source separation and detailed explanations.

## 📚 Table of Contents

1. [Introduction to SystemC](#introduction-to-systemc)
2. [Basic Concepts](#basic-concepts)
3. [Process Types](#process-types)
4. [Event Handling](#event-handling)
5. [Advanced Topics](#advanced-topics)
6. [SystemC RTL Examples](#systemc-rtl-examples)

---

## Introduction to SystemC

| # | Example | Description | Key Concepts |
|---|---------|-------------|--------------|
| 01 | [Hello World](SystemC_RTL/00_Hello_World/01_Hello_World.md) | First SystemC example | `sc_main`, `SC_MODULE`, `SC_THREAD`, `sc_start` |

---

## Basic Concepts

| # | Example | Description | Key Concepts |
|---|---------|-------------|--------------|
| 02 | [Module Creation Approaches](basic/module_creation.md) | Three ways to create modules | `SC_MODULE` macro, struct inheritance, class inheritance |
| 03 | [Time Resolution and Units](basic/time_resolution.md) | Configuring simulation time | `sc_set_time_resolution`, `sc_set_default_time_unit`, time conversions |
| 04 | [Elaboration and Simulation Callbacks](basic/callbacks.md) | Lifecycle callbacks | `before_end_of_elaboration`, `end_of_elaboration`, `start_of_simulation`, `end_of_simulation` |

---

## Process Types

| # | Example | Description | Key Concepts |
|---|---------|-------------|--------------|
| 05 | [SC_METHOD Process](process_types/sc_method.md) | Lightweight process type | `SC_METHOD`, `next_trigger`, run-to-completion |
| 06 | [Three Process Types](process_types/three_processes.md) | Comparison of all process types | `SC_METHOD`, `SC_THREAD`, `SC_CTHREAD`, clock synchronization |
| 07 | [Concurrency and Cooperative Multitasking](process_types/concurrency.md) | Simulating parallel execution | Cooperative multitasking, yielding, `wait()` |
| 08 | [Static vs Dynamic Sensitivity](process_types/sensitivity.md) | Process sensitivity mechanisms | Static sensitivity, dynamic sensitivity, `sensitive << event` |

---

## Event Handling

| # | Example | Description | Key Concepts |
|---|---------|-------------|--------------|
| 09 | [Ordered Events](events/ordered_events.md) | Event-based synchronization | `sc_event`, `wait()`, `notify()`, circular dependencies |
| 10 | [Event Hogs](events/event_hogs.md) | Immediate notifications | Immediate `notify()`, process scheduling, `sc_stop()` |
| 11 | [Missing Events](events/missing_events.md) | Event synchronization pitfalls | Event race conditions, `notify(SC_ZERO_TIME)` |
| 12 | [Combined Events](events/combined_events.md) | Advanced event combinations | AND/OR combinations, timeouts, complex conditions |
| 13 | [Delta Cycles](events/delta_cycles.md) | Process ordering and variable visibility | Delta cycles, evaluation phase, update phase |

---

## SystemC RTL Examples

Hardware-oriented examples showing RTL modeling with SystemC.

| # | Example | Description | Key Concepts |
|---|---------|-------------|--------------|
| 14 | [Counter with SC_CTHREAD](SystemC_RTL/counter.md) | Clocked counter | `SC_CTHREAD`, clock sensitivity, reset logic |
| 15 | [FIFO with SC_THREAD](SystemC_RTL/fifo.md) | FIFO buffer implementation | `SC_THREAD`, handshaking, bounded buffers |
| 16 | [Adder with SC_METHOD](SystemC_RTL/adder.md) | Combinational adder | `SC_METHOD`, combinational logic, sensitivity list |

---

## Example Details

### SystemC_RTL/00_Hello_World/01_Hello_World.md

```markdown
# 01: SystemC Hello World

**Files:** `hello1.h`, `hello1.cpp`, `hello_world.h`, `hello_world.cpp`, `main.cpp`

This example demonstrates the fundamental difference between normal C++ functions and SystemC processes.

## Key Concepts

- **sc_main**: SystemC entry point
- **SC_MODULE**: Hardware module definition
- **SC_CTOR**: Module constructor
- **SC_THREAD**: Process registration
- **sc_start**: Simulation kernel activation

## Code Structure
