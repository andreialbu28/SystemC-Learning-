---
title: "Section 1 — AMS Foundations"
description: Set up SystemC AMS, write your first TDF module, understand the two parallel time models, and record signals to trace files.
tags:
  - AMS
  - TDF
  - foundations
---

# Section 1 — AMS Foundations

**Folder:** `examples/SystemC_AMS/20_AMS_Foundations/`

Three short examples that establish everything you need before writing real
signal processing. If you're comfortable with `SC_MODULE` and `sc_signal`
from the RTL/TLM series, each example here has a direct analogy:

| Digital concept | AMS equivalent | Where introduced |
|----------------|----------------|-----------------|
| `SC_MODULE` + `SC_CTOR` | `SCA_TDF_MODULE` + `SCA_CTOR` | Example 01 |
| `SC_METHOD` body | `processing()` | Example 01 |
| `sc_signal<T>` | `sca_tdf::sca_signal<T>` | Example 01 |
| Sensitivity list | `set_timestep()` in `set_attributes()` | Example 01 |
| `sc_signal<T>` driving an AMS module | `sca_tdf::sca_de_in<T>` bridge port | Example 02 |
| `sc_trace()` + VCD | `sca_trace()` + VCD / tabular | Example 03 |

---

## Examples

<div class="grid cards" markdown>

-   :material-numeric-1-circle: **[01 — AMS Hello World](01_ams_hello_world.md)**

    ---
    `SCA_TDF_MODULE` · `SCA_CTOR` · `set_attributes()` · `processing()` ·
    `sca_tdf::sca_signal<T>`

    The minimal TDF source → sink chain. Every AMS simulation starts here.

-   :material-numeric-2-circle: **[02 — Time in AMS](02_time_in_ams.md)**

    ---
    `sca_tdf::sca_de_in<T>` · timestep synchronization · aliasing

    A digital SC_THREAD and a TDF sampler run side by side.
    See exactly when the TDF sampler captures digital values — and when it misses them.

-   :material-numeric-3-circle: **[03 — AMS Trace](03_ams_trace.md)**

    ---
    `sca_util::sca_trace()` · VCD files · tabular `.dat` files · GTKWave · Python plotting

    Record a 1 MHz sine wave to both trace formats. Includes a ready-to-run
    Python plotting script.

</div>

---

## Build all Section 1 examples

```bash
# From repo root
make -C examples/SystemC_AMS/20_AMS_Foundations all
```

Or build individually (see the Makefile in that folder for the full commands):

```bash
make -C examples/SystemC_AMS/20_AMS_Foundations 01_ams_hello_world
make -C examples/SystemC_AMS/20_AMS_Foundations 02_time_in_ams
make -C examples/SystemC_AMS/20_AMS_Foundations 03_ams_trace
```

---

## Section summary — callbacks and their purpose

Every TDF module can implement four callbacks. In this section you've used two:

| Callback | When called | Use for |
|----------|-------------|---------|
| `set_attributes()` | Elaboration | `set_timestep()`, `set_rate()`, `set_delay()` |
| `initialize()` | Before simulation starts | Set initial output values, pre-load filter state |
| `processing()` | Every timestep | Per-sample computation |
| `ac_processing()` | AC analysis (optional) | Frequency-domain behavior |

`initialize()` and `ac_processing()` are introduced in Section 2.

---

**Next:** [Section 2 — TDF: Timed Data Flow →](../21_TDF_Timed_Data_Flow/index.md)