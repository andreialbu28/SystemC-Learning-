# SystemC Hello World

A minimal introductory example demonstrating two fundamental approaches to executing code in SystemC.

## Overview

SystemC is a C++ library that extends the language with hardware modeling and simulation constructs. This example shows the contrast between a plain C++ function call and a SystemC **module** with a **method process** — the two most basic building blocks you'll encounter in any SystemC design.

## Project Structure

```
├── include/
│   └── hello_world.h       # Module declaration
├── src/
│   ├── hello_world.cpp     # Module implementation
│   └── main.cpp            # Entry point (sc_main)
└── test.sh                 # Build and run script
```

## The Two Approaches

### Approach 1 — Normal C++ Function (`hello1`)

```cpp
void hello1() {
    std::cout << "Hello world using Normal C++ function" << std::endl;
}
```

This is just a regular C++ function. It runs immediately when called, outside of any simulation context. There is no notion of time, scheduling, or simulation events.

### Approach 2 — SystemC Module with a Method Process (`hello2`)

```cpp
SC_MODULE(HelloWorld) {
    SC_CTOR(HelloWorld);
    void hello2();
};
```

`HelloWorld` is a **SystemC module** — the basic unit of hierarchy in a SystemC design, roughly analogous to a hardware block or component. Inside its constructor, `hello2` is registered as an `SC_METHOD` process:

```cpp
HelloWorld::HelloWorld(sc_module_name name) {
    SC_METHOD(hello2);
}
```

An `SC_METHOD` is a process managed by the SystemC simulation kernel. Rather than being called directly, it is **scheduled and invoked by the kernel** when `sc_start()` is called in `sc_main`. This is the fundamental mechanism by which SystemC models concurrent, event-driven hardware behavior.

## How It Runs

```
sc_main()
  │
  ├─ hello1()          ← called directly, prints immediately
  │
  ├─ HelloWorld helloworld("helloworld")   ← module instantiated
  │
  └─ sc_start()        ← simulation kernel starts
        └─ invokes helloworld.hello2()     ← SC_METHOD executes
```

Expected output:
```
Hello world using Normal C++ function
Hello world using SystemC method process
```

## Prerequisites

- A C++17-compatible compiler (g++)
- SystemC 3.0.2 installed under `external/systemc-3.0.2/` relative to the project root

## Building and Running

Use the provided `test.sh` script from the example directory:

```bash
# Build
./test.sh all

# Run
./test.sh run

# Clean build artifacts
./test.sh clean
```

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_main` | The SystemC entry point, replacing `main` |
| `SC_MODULE` | Macro to declare a SystemC module (a class derived from `sc_module`) |
| `SC_CTOR` | Macro to declare a module constructor |
| `SC_METHOD` | Registers a member function as a method process with the simulation kernel |
| `sc_start()` | Starts the simulation; the kernel runs all registered processes |

## Next Steps

From here, the natural progression is to explore:

- [02_SC_MODULE.md](02_SC_MODULE.md): 