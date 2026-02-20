# SystemC Signals — Resolved Signal (`05_resolved_signal`)

This example introduces **`sc_signal_resolved`** — a multi-driver signal channel that uses four-valued logic and a resolution table to determine the output when multiple processes drive it simultaneously.

## Overview

The previous sub-example (`04_multi_writer`) used `SC_MANY_WRITERS` with integer signals, where last-write-wins determines the result. For hardware modeling, this is often not realistic — real buses resolve contention according to electrical properties. `sc_signal_resolved` models this properly: when multiple drivers write different `sc_logic` values simultaneously, a **resolution function** combines them into a single deterministic output value.

## Four-Valued Logic — `sc_logic`

SystemC's `sc_dt::sc_logic` models IEEE 1164 four-valued logic, the standard used in VHDL and hardware simulation:

| Constant | Value | Meaning |
|---|---|---|
| `sc_logic_0` | `'0'` | Strong logic zero |
| `sc_logic_1` | `'1'` | Strong logic one |
| `sc_logic_Z` | `'Z'` | High impedance (tri-state / not driven) |
| `sc_logic_X` | `'X'` | Unknown / conflict |

These four values allow the simulator to model tri-state buses, unconnected nets, and contention — things that a plain `bool` cannot represent.

## Resolution Table

When two drivers write to an `sc_signal_resolved` in the same delta cycle, the channel applies the following resolution table to produce a single output:

| Driver A ↓ \ Driver B → | `0` | `1` | `Z` | `X` |
|---|---|---|---|---|
| **`0`** | `0` | `X` | `0` | `X` |
| **`1`** | `X` | `1` | `1` | `X` |
| **`Z`** | `0` | `1` | `Z` | `X` |
| **`X`** | `X` | `X` | `X` | `X` |

Key rules:
- Same value + same value → that value (`0`+`0`=`0`, `1`+`1`=`1`, `Z`+`Z`=`Z`)
- Any value + `Z` → that value (`Z` is high-impedance, doesn't interfere)
- `0` + `1` → `X` (contention — two strong drivers fighting)
- Anything + `X` → `X` (unknown propagates)

## The Module

```cpp
sc_signal_resolved rv;
vector<sc_logic> levels = {sc_logic_0, sc_logic_1, sc_logic_Z, sc_logic_X};
```

Two writers cycle through all 16 combinations of the four logic values:

### `writer1` — cycles through values per second
```
Pattern: 0, 1, Z, X, 0, 1, Z, X, ...  (changes every second)
```

### `writer2` — holds each value for 4 seconds
```
Pattern: 0, 0, 0, 0, 1, 1, 1, 1, Z, Z, Z, Z, X, X, X, X  (changes every 4 seconds)
```

Together they produce all 16 combinations of (writer1_value, writer2_value) across 16 seconds.

### `consumer` — reads and prints
```cpp
void consumer() {
    wait(1, SC_SEC);  // delay 1 s — skip time 0
    while (true) {
        std::cout << " " << rv.read() << " |";
        if (++idx % 4 == 0) cout << endl;
        wait(1, SC_SEC);
    }
}
```

The consumer delays 1 second before starting, then reads the resolved value every second, printing 4 values per line — one row per `writer2` value.

## Expected Output — Full Resolution Table

The 16 combinations map directly onto the resolution table. Reading left to right, top to bottom:

```
 0  |  X  |  0  |  X  |     ← writer2='0': 0+0=0, 0+1=X, 0+Z=0, 0+X=X
 X  |  1  |  1  |  X  |     ← writer2='1': 1+0=X, 1+1=1, 1+Z=1, 1+X=X
 0  |  1  |  Z  |  X  |     ← writer2='Z': Z+0=0, Z+1=1, Z+Z=Z, Z+X=X
 X  |  X  |  X  |  X  |     ← writer2='X': X+anything=X
```

This output is exactly the standard IEEE four-valued logic resolution table — the example is a live demonstration of it.

## `sc_signal_resolved` vs. `sc_signal<SC_MANY_WRITERS>`

| | `sc_signal<T, SC_MANY_WRITERS>` | `sc_signal_resolved` |
|---|---|---|
| **Value type** | Any `T` | `sc_logic` only |
| **Multi-driver rule** | Last write wins | Resolution table |
| **Models** | Software arbitration | Hardware bus/net physics |
| **Deterministic?** | Depends on execution order | Yes — table is commutative |
| **Use when** | Multiple writers, one wins | Tri-state buses, wired logic |

The resolution table is **commutative and associative**, meaning the result is the same regardless of which driver writes first or how many drivers are present — a key property for simulation correctness.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `sc_signal_resolved` | Multi-driver signal using four-valued logic resolution |
| `sc_logic` | Four-valued type: `0`, `1`, `Z`, `X` |
| `sc_logic_0/1/Z/X` | The four logic level constants |
| Resolution table | Deterministic rule combining two `sc_logic` values into one |
| `Z` (high impedance) | Transparent — does not interfere with the other driver's value |
| `X` (unknown) | Contention or unknown — propagates through any resolution |
| `sc_dt` namespace | Where `sc_logic` and related types are defined |

## Relation to Previous Sub-Examples

`04_multi_writer` showed that `SC_MANY_WRITERS` allows multiple integer writers with last-write-wins semantics — a software-level solution. This example replaces that with a hardware-accurate model: `sc_signal_resolved` uses physics-inspired resolution so the result is independent of simulation execution order. It also introduces `sc_logic` as a prerequisite for understanding `sc_lv` (logic vectors) used in RTL-level modeling.

## What Comes Next

The next topic is **ports** (`sc_in`, `sc_out`, `sc_inout`) — the mechanism for exposing signals at module boundaries and wiring module instances together in a structural design hierarchy.