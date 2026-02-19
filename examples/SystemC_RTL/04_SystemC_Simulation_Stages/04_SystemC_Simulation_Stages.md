# SystemC Elaboration & Simulation Callbacks

This example explores the **lifecycle of a SystemC simulation**, showing the four callback functions the kernel invokes at well-defined transition points between phases, and how they map onto the module's constructor, processes, and destructor.

## Overview

A SystemC simulation does not jump straight from construction to running. It passes through a sequence of distinct phases — **Elaboration**, **Execution** (split into Initialization and Simulation), and **Cleanup** — and the kernel provides four virtual callback hooks that let a module react to each transition. Understanding this lifecycle is essential for building correct, well-structured SystemC models.

## Simulation Phases

```
sc_main() called
│
├─ [ELABORATION]
│   ├─ Module constructors run
│   ├─ before_end_of_elaboration() ← callback 1
│   └─ end_of_elaboration()        ← callback 2
│
sc_start() called
│
├─ [EXECUTION — Initialization]
│   ├─ start_of_simulation()       ← callback 3
│   └─ All SC_THREADs run until their first wait()
│
├─ [EXECUTION — Simulation]
│   └─ Kernel advances time, triggers processes repeatedly
│
sc_stop() called
│
├─ end_of_simulation()             ← callback 4
│
return 0 (sc_main exits)
│
└─ [CLEANUP]
    └─ Module destructors run
```

## The Four Callbacks

All four are virtual member functions inherited from `sc_module`. You override them in your module to inject behavior at each transition point.

```cpp
virtual void before_end_of_elaboration();
virtual void end_of_elaboration();
virtual void start_of_simulation();
virtual void end_of_simulation();
```

They are defined with empty bodies by default, so you only need to override the ones you care about. They are available not just in `sc_module` but also in `sc_port`, `sc_export`, and `sc_prim_channel`.

---

### `before_end_of_elaboration`

Called after the module hierarchy is fully constructed but before elaboration is finalized. This is the only callback that may **modify the module hierarchy** — for example, by dynamically instantiating additional submodules or binding ports based on the global structure of the design. It is the hook for actions that depend on knowing the whole hierarchy but still need to change it.

---

### `end_of_elaboration`

Called once the hierarchy is complete and frozen. At this point the model is fully built and no further structural changes are allowed. This is a good place to perform final checks or print a summary of the elaborated design.

---

### `start_of_simulation`

Called by the kernel immediately before the first delta cycle of simulation begins (i.e. just before threads run their initialization code). Use this to set up initial signal values, open trace files, or perform any one-time initialization that should happen at time zero but after elaboration is confirmed complete.

---

### `end_of_simulation`

Called when `sc_stop()` is invoked (either by a process or externally) before the kernel shuts down. This is the right place to close files, flush buffers, print simulation statistics, or perform any final teardown actions.

---

## The Module in This Example

```cpp
SC_MODULE(STAGE) {
    SC_CTOR(STAGE) {
        std::cout << sc_time_stamp() << ": Elaboration: constructor" << std::endl;
        SC_THREAD(thread);
    }

    ~STAGE() {
        std::cout << sc_time_stamp() << ": Cleanup: destructor" << std::endl;
    }

    void thread() {
        std::cout << sc_time_stamp() << ": Execution.initialization" << std::endl;
        int i = 0;
        while (true) {
            wait(1, SC_SEC);
            std::cout << sc_time_stamp() << ": Execution.simulation" << std::endl;
            if (++i >= 2) sc_stop();
        }
    }

    void before_end_of_elaboration() { ... }
    void end_of_elaboration()        { ... }
    void start_of_simulation()       { ... }
    void end_of_simulation()         { ... }
};
```

The `thread` runs its initialization code (the part before the first `wait()`) during the Initialization phase, then re-enters the loop every second. After 2 iterations it calls `sc_stop()`, which triggers `end_of_simulation()` and ends the run.

Note that `sc_start()` is called **without** a time argument here, meaning the simulation runs until `sc_stop()` is explicitly called from within a process.

## Expected Output

```
0 s: Elaboration: constructor
before end of elaboration
end of elaboration
start of simulation
0 s: Execution.initialization
1 s: Execution.simulation
i = 0
2 s: Execution.simulation
i = 1
i = 2
end of simulation
0 s: Cleanup: destructor
```

A few things worth noting in this output:

- The constructor and both elaboration callbacks all print `0 s` — no simulation time has advanced yet during elaboration.
- The destructor also prints `0 s` because simulation time is reset/irrelevant at cleanup; the destructor runs after `sc_main` returns, outside the simulation context.
- `i = 2` is printed before `end of simulation` because `sc_stop()` is called from within the thread body — the callback fires as part of the stop sequence before the kernel fully shuts down.

## Phase-to-Code Mapping

| Phase | Code responsible |
|---|---|
| Elaboration | `STAGE stage("stage")` — constructor runs |
| before_end_of_elaboration | Kernel calls `before_end_of_elaboration()` |
| end_of_elaboration | Kernel calls `end_of_elaboration()` |
| Initialization | `sc_start()` — thread runs to first `wait()` |
| start_of_simulation | Kernel calls `start_of_simulation()` before delta 0 |
| Simulation | Kernel advances time, thread wakes each second |
| end_of_simulation | `sc_stop()` inside thread triggers the callback |
| Cleanup | `return 0` in `sc_main` — destructor runs |

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `before_end_of_elaboration()` | Post-construction hook; may still modify the hierarchy |
| `end_of_elaboration()` | Hierarchy is frozen; inspection only |
| `start_of_simulation()` | Fires just before time-zero initialization |
| `end_of_simulation()` | Fires when `sc_stop()` is called |
| `sc_start()` (no args) | Runs simulation indefinitely until `sc_stop()` |
| `sc_stop()` | Halts simulation and triggers `end_of_simulation()` |
| Destructor | Runs during cleanup after `sc_main` returns, not during simulation |

## What Comes Next

With the full simulation lifecycle understood, the next topic is **time notation**. 