# SystemC Module Declaration Approaches

A focused example demonstrating the **three equivalent ways to declare a SystemC module** in C++. All three produce identical results — the difference is purely syntactic and stylistic.

## Overview

In SystemC, a module is the fundamental building block for modeling hardware components. Under the hood, every module is a C++ class that inherits from `sc_module`. This example shows the three ways to express that, and how the `SC_CTOR` macro works uniformly across all of them.

## Project Structure

```
├── include/
│   ├── module_A.h      # Approach 1: SC_MODULE macro
│   ├── module_B.h      # Approach 2: struct inheriting sc_module
│   └── module_C.h      # Approach 3: class inheriting sc_module
├── src/
│   ├── module_A.cpp
│   ├── module_B.cpp
│   ├── module_C.cpp
│   └── main.cpp
```

## The Three Approaches

### Approach 1 — `SC_MODULE` Macro (most common in SystemC code)

```cpp
SC_MODULE(MODULE_A) {
    SC_CTOR(MODULE_A);
};
```

`SC_MODULE(MODULE_A)` is a convenience macro that expands to:

```cpp
struct MODULE_A : public sc_module
```

It is the most idiomatic and widely used style in SystemC designs. Because it expands to a `struct`, all members are `public` by default.

---

### Approach 2 — `struct` inheriting `sc_module` (explicit struct)

```cpp
struct MODULE_B : public sc_module {
    SC_CTOR(MODULE_B);
};
```

This is exactly what `SC_MODULE` expands to, written out explicitly. It is fully equivalent to Approach 1. Some developers prefer this style for clarity — making the inheritance relationship from `sc_module` visible without relying on a macro.

---

### Approach 3 — `class` inheriting `sc_module` (explicit class)

```cpp
class MODULE_C : public sc_module {
public:
    SC_CTOR(MODULE_C);
};
```

Functionally identical to the previous two approaches, but uses `class` instead of `struct`. Because `class` defaults to `private` access, the `public:` specifier is required. This style is preferred by developers who follow standard C++ conventions of using `class` for all user-defined types.

---

## Comparison Summary

| | Approach 1 | Approach 2 | Approach 3 |
|---|---|---|---|
| **Syntax** | `SC_MODULE(NAME)` | `struct NAME : sc_module` | `class NAME : sc_module` |
| **Default member access** | `public` | `public` | `private` |
| **`public:` needed?** | No | No | Yes |
| **Idiomatic SystemC?** | ✅ Most common | Acceptable | Acceptable |
| **Behavior** | Identical | Identical | Identical |

## The `SC_CTOR` Macro

All three approaches use `SC_CTOR` to declare the constructor. It expands to a constructor that accepts an `sc_module_name` argument and passes it to the `sc_module` base class:

```cpp
MODULE_A(sc_module_name name) : sc_module(name)
```

This is how the SystemC kernel receives and registers the instance name you pass at instantiation time in `sc_main` (e.g. `"Module_A"`), making it available at runtime via the `name()` method.

## Expected Output

```
Module_A constructor using the first approach
Module_B constructor using the second approach
Module_C constructor using the third approach
```

The constructors run during **elaboration** — before `sc_start()` is called — as the kernel builds up the module hierarchy.

## Relation to the Previous Example

The previous example (`00_Hello_World`) showed a module that registered an `SC_METHOD` process inside its constructor body. This example steps back to focus on **how the module class itself is declared**, which is the foundation every SystemC module builds on regardless of what processes it contains.

## What Comes Next

The next example [02_SC_CTOR](../02_SC_CTOR/02_SC_CTOR.md) goes deeper into constructor mechanics, exploring how to write constructors without the `SC_CTOR` macro, how to pass additional arguments, and how to initialize members and register processes within the constructor body.