# SystemC Signals — Multiple Writers (`04_multi_writer`)

This example introduces **`sc_signal<T, SC_MANY_WRITERS>`** — the multi-writer variant of `sc_signal` — and shows what happens when two processes write to the same signal in the same delta cycle, contrasting it with the default single-driver `sc_signal` that enforces exclusive ownership.

## Overview

By default, `sc_signal` enforces a **single-writer policy**: if two processes write to the same signal in the same delta cycle, the simulator reports an error. This reflects the hardware reality that a net driven by multiple sources without arbitration is a short circuit. When multiple drivers are genuinely intended — as in a bus or wired-OR topology — `SC_MANY_WRITERS` relaxes this restriction, applying a last-write-wins rule instead.

## The Two Signal Types

```cpp
sc_signal<int>                  s1;  // single writer — error if multiple writers in same delta
sc_signal<int, SC_MANY_WRITERS> s2;  // multi-writer — last write in delta wins, no error
```

The second template argument is the **writer policy**:

| Policy | Default? | Behaviour |
|---|---|---|
| `SC_ONE_WRITER` | ✅ Yes | Error if more than one process writes per delta |
| `SC_MANY_WRITERS` | No | Last write in delta wins; no error |

## The Module

Two writers and two consumers:

```cpp
SC_THREAD(writer_1);   // writes s1 and s2 every 1 s, starting at 0 s
SC_THREAD(writer_2);   // writes s2 only every 1 s, starting after SC_ZERO_TIME
```

`writer_1` is the **sole writer of `s1`** — this is safe with the default policy. Both writers write to `s2`, which requires `SC_MANY_WRITERS`.

```cpp
SC_THREAD(consumer_1);
sensitive << s1;          // wakes only when s1 changes

SC_THREAD(consumer_2);
sensitive << s1 << s2;    // wakes when s1 OR s2 changes
```

## Writer Timing — The `SC_ZERO_TIME` Offset

`writer_2` begins with a delta delay:

```cpp
void writer_2() {
    int v = 15;
    while (true) {
        wait(SC_ZERO_TIME);  // delay one delta before writing
        s2.write(v);
        wait(1, SC_SEC);
    }
}
```

This is deliberate. Without the delta delay, both writers would write to `s2` in the **same delta** at time 0, and `writer_2`'s value would overwrite `writer_1`'s (last-write-wins). By waiting one delta first, `writer_2` writes in the *next* delta — so at time 0 `s2` first takes `writer_1`'s value, then `writer_2`'s value one delta later. From time 1 s onward both writers are in phase with each other (one delta apart within each second).

## Execution at Time 0

```
Delta 0:
  writer_1 writes s1=10, s2=10
  (writer_2 delays via SC_ZERO_TIME)

Delta 1:
  s1 and s2 commit (value = 10) → consumer_1 and consumer_2 wake
  writer_2 writes s2=15

Delta 2:
  s2 commits (value = 15) → consumer_2 wakes (s2 changed again)
  consumer_1 does NOT wake (s1 unchanged)
```

## Why `s1` Is Safe Without `SC_MANY_WRITERS`

Only `writer_1` writes to `s1`. The single-writer policy is satisfied. If `writer_2` were to also write `s1`, the simulator would throw a runtime error:

```
Error: (E115) sc_signal<T> cannot have more than one driver
```

This error is a design-time safety net — it flags structural mistakes in module connectivity before they produce subtle simulation bugs.

## Consumer Sensitivity Difference

| Consumer | Sensitive to | Wakes when |
|---|---|---|
| `consumer_1` | `s1` only | `s1` changes value |
| `consumer_2` | `s1` and `s2` | Either `s1` or `s2` changes value |

Because `s2` changes in an extra delta (when `writer_2` overrides `writer_1`'s value), `consumer_2` wakes more often than `consumer_1`.

## Expected Output (first cycle)

```
@ time 0 s: WRITER 1 writes: 10
@ time 0 s: CONSUMER 1 reads s1=10; s2=10
@ time 0 s: CONSUMER 2 reads s1=10; s2=10
@ time 0 s: WRITER 2 writes: 15
@ time 0 s: CONSUMER 2 reads s1=10; s2=15     ← s2 changed again, consumer_2 wakes
@ time 1 s: WRITER 1 writes: 11
@ time 1 s: CONSUMER 1 reads s1=11; s2=14
@ time 1 s: CONSUMER 2 reads s1=11; s2=14
@ time 1 s: WRITER 2 writes: 14
@ time 1 s: CONSUMER 2 reads s1=11; s2=14
...
```

`consumer_1` wakes once per second (only `s1` changes). `consumer_2` wakes twice per second — once when `s1` changes (written by `writer_1`) and once when `s2` gets its second write from `writer_2` a delta later.

## Key Concepts Introduced

| Concept | Description |
|---|---|
| `SC_ONE_WRITER` | Default policy — runtime error if multiple writers in same delta |
| `SC_MANY_WRITERS` | Relaxed policy — last write in delta wins, no error |
| Last-write-wins | When multiple writes occur in the same delta, the final one is committed |
| Single-writer enforcement | Protects against accidental multi-driver bugs in structural designs |
| Delta offset between writers | `wait(SC_ZERO_TIME)` staggers two writers so they write in different deltas |
| Differential sensitivity | Two consumers on the same signals can have different sensitivity sets |

## When to Use `SC_MANY_WRITERS`

Use `SC_MANY_WRITERS` only when the hardware model genuinely has multiple drivers — for example:
- Tri-state / open-drain buses where multiple modules drive a shared net
- Wired-OR or wired-AND logic
- Arbitrated bus models

For all other signals, keep the default `SC_ONE_WRITER`. The error it produces on accidental multi-driver situations is a valuable design checker.

## Relation to Previous Sub-Examples

Previous sub-examples always had one writer per signal. This example is the first to introduce multiple writers and explains why the default policy exists and when to override it. The `SC_ZERO_TIME` stagger technique also revisits the delta-ordering lessons from `08_02_Missing_Event` and `11_Mutex`.

## What Comes Next

The next topic is **ports** (`sc_in`, `sc_out`, `sc_inout`) — the mechanism for exposing signals as typed interface points on module boundaries, enabling structural connectivity between separate module instances.