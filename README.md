# Standard 64-bit Bytecode Interpreter

A small, clean, safety-conscious **64-bit stack-based bytecode interpreter written in C11**.

The project is designed as a foundation for a real programming-language runtime. It separates the virtual machine from the bytecode-generation layer so that a lexer, parser, AST, and compiler can be added later without rewriting the VM.

---

## Features

* 64-bit signed VM values
* Stack-based execution model
* Dedicated instruction pointer (`IP`)
* Dedicated frame pointer (`FP`)
* Accumulator register (`AX`)
* Separate bytecode, stack, and heap
* Stack overflow/underflow checks
* Bytecode bounds checking
* Heap bounds checking
* Safe division and modulo
* Arithmetic operations
* Bitwise operations
* Comparison operations
* Conditional jumps
* Unconditional jumps
* Function calls and returns
* Local stack frames
* 8-bit and 64-bit memory operations
* VM heap allocation
* `memset` support
* `memcmp` support
* Integer output
* Cycle counter
* Clean VM initialization/destruction
* No unsafe `#define int ...` hacks
* Standard C11 integer types

---

## Project Structure

```text
interpreter/
│
├── README.md
│
└── interpreter.c
```

The project intentionally starts small.

The VM is implemented in:

```text
interpreter.c
```

Documentation and build instructions are contained in:

```text
README.md
```

---

# Architecture

The interpreter is divided conceptually into several layers.

```text
             Source Code
                  │
                  ▼
              Lexer
                  │
                  ▼
              Parser
                  │
                  ▼
                AST
                  │
                  ▼
           Bytecode Compiler
                  │
                  ▼
             Bytecode
                  │
                  ▼
        ┌──────────────────┐
        │       VM         │
        │                  │
        │ IP               │
        │ FP               │
        │ SP               │
        │ AX               │
        │                  │
        │ Stack            │
        │ Heap             │
        └──────────────────┘
                  │
                  ▼
              Execution
```

The current implementation provides the **bytecode VM** and a small bytecode builder used to demonstrate it.

---

# Virtual Machine

The VM contains four important execution registers.

## Instruction Pointer

```text
IP
```

Points to the next bytecode instruction.

---

## Stack Pointer

```text
SP
```

Points to the next available stack position.

The VM uses a conventional value stack for operands and intermediate results.

---

## Frame Pointer

```text
FP
```

Identifies the current function stack frame.

This allows functions and local variables to be implemented without relying on native C function calls.

---

## Accumulator

```text
AX
```

Stores the VM's most recently calculated result.

---

# Memory Model

The VM has three major memory areas.

```text
┌──────────────────────────┐
│       BYTECODE           │
│                          │
│ instructions + operands  │
└──────────────────────────┘

┌──────────────────────────┐
│         STACK            │
│                          │
│ operands                 │
│ temporary values         │
│ call frames              │
│ local variables          │
└──────────────────────────┘

┌──────────────────────────┐
│          HEAP            │
│                          │
│ dynamically allocated    │
│ VM memory                │
└──────────────────────────┘
```

The VM does **not** use arbitrary native pointers as bytecode addresses.

Heap addresses are represented as offsets into the VM heap.

This makes memory operations considerably easier to validate.

---

# Instruction Set

## Program Control

| Opcode    | Description               |
| --------- | ------------------------- |
| `OP_HALT` | Stop execution            |
| `OP_JMP`  | Unconditional jump        |
| `OP_JZ`   | Jump if value is zero     |
| `OP_JNZ`  | Jump if value is non-zero |

---

## Stack

| Opcode     | Description         |
| ---------- | ------------------- |
| `OP_PUSH`  | Push `AX`           |
| `OP_POP`   | Pop into `AX`       |
| `OP_DUP`   | Duplicate stack top |
| `OP_CONST` | Push constant       |

---

## Arithmetic

| Opcode   | Operation      |
| -------- | -------------- |
| `OP_ADD` | Addition       |
| `OP_SUB` | Subtraction    |
| `OP_MUL` | Multiplication |
| `OP_DIV` | Division       |
| `OP_MOD` | Modulo         |

For example:

```text
10 + 20
```

becomes conceptually:

```text
CONST 10
CONST 20
ADD
```

---

## Bitwise Operations

| Opcode   | Operation   |
| -------- | ----------- |
| `OP_OR`  | Bitwise OR  |
| `OP_XOR` | Bitwise XOR |
| `OP_AND` | Bitwise AND |
| `OP_SHL` | Shift left  |
| `OP_SHR` | Shift right |

---

## Comparisons

| Opcode  | Operation             |
| ------- | --------------------- |
| `OP_EQ` | Equal                 |
| `OP_NE` | Not equal             |
| `OP_LT` | Less than             |
| `OP_GT` | Greater than          |
| `OP_LE` | Less than or equal    |
| `OP_GE` | Greater than or equal |

Comparison results are:

```text
1 = true
0 = false
```

---

# Functions

The VM supports:

```text
OP_CALL
OP_RET
OP_ENTER
OP_LEAVE
```

A conceptual function call looks like:

```text
CALL function
```

The VM stores the return address and previous frame pointer.

A function can then create its local frame:

```text
ENTER number_of_locals
```

and eventually return using:

```text
RET
```

This establishes the foundation for compiled functions.

---

# Memory Operations

The VM supports:

```text
OP_LOAD8
OP_LOAD64
OP_STORE8
OP_STORE64
```

Memory addresses refer to offsets inside the VM heap.

For example:

```text
address = 100
```

means:

```text
heap[100]
```

rather than an arbitrary native process address.

This provides a much safer abstraction for the language runtime.

---

# Heap

Dynamic allocation is provided through:

```text
OP_MALLOC
```

The current allocator is intentionally simple: it uses a monotonic allocation strategy.

That means allocated memory is consumed from the VM heap but is not currently returned to the allocator.

`OP_FREE` exists as an instruction boundary for future allocator improvements.

A production allocator could later replace this with:

* free lists
* block coalescing
* size classes
* garbage collection
* reference counting
* arena allocation

without changing the rest of the interpreter architecture.

---

# Example Program

The included test program represents:

```text
print(10 + 20);
print(100 / 4);
print(7 * 8);
```

Its bytecode is conceptually:

```text
CONST 10
CONST 20
ADD
PRINT

CONST 100
CONST 4
DIV
PRINT

CONST 7
CONST 8
MUL
PRINT

HALT
```

Expected output:

```text
30
25
56
```

---

# Building

## GCC

```bash
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 interpreter.c -o interpreter
```

Run:

```bash
./interpreter
```

---

## Clang

```bash
clang -std=c11 -Wall -Wextra -Wpedantic -O2 interpreter.c -o interpreter
```

Run:

```bash
./interpreter
```

---

## Windows / MinGW

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -O2 interpreter.c -o interpreter.exe
```

Run:

```powershell
.\interpreter.exe
```

---

# Expected Output

```text
========================================
 Standard 64-bit Bytecode Interpreter
========================================

30
25
56

----------------------------------------
Execution finished
Cycles: ...
Exit code: 0
Final AX: 56
----------------------------------------
```

The exact cycle count can change as the instruction set evolves.

---

# Safety

The interpreter performs checks for:

* stack overflow
* stack underflow
* invalid instruction pointers
* invalid bytecode addresses
* truncated operands
* invalid heap addresses
* heap buffer overflow
* division by zero
* modulo by zero
* signed division overflow
* invalid shift amounts
* invalid function frames
* invalid return addresses
* unknown opcodes

This is important because a VM should not blindly trust its bytecode.

---

# Current Limitations

This project is currently a **bytecode virtual machine**, not a complete programming language.

It does not yet contain:

* lexer
* parser
* AST
* source-code compiler
* variables by name
* strings
* arrays
* structures
* objects
* modules
* garbage collection
* exceptions
* standard library
* debugger
* bytecode file format
* bytecode verifier
* JIT compiler

These can be added on top of the current VM.

---

# Recommended Future Architecture

The long-term project should evolve toward:

```text
                  ┌─────────────┐
                  │ Source Code │
                  └──────┬──────┘
                         │
                         ▼
                  ┌─────────────┐
                  │    Lexer    │
                  └──────┬──────┘
                         │
                         ▼
                  ┌─────────────┐
                  │    Parser   │
                  └──────┬──────┘
                         │
                         ▼
                  ┌─────────────┐
                  │     AST     │
                  └──────┬──────┘
                         │
                         ▼
                ┌─────────────────┐
                │ Bytecode Compiler│
                └────────┬────────┘
                         │
                         ▼
                  ┌─────────────┐
                  │  Bytecode   │
                  └──────┬──────┘
                         │
                         ▼
              ┌──────────────────────┐
              │    Bytecode VM       │
              │                      │
              │ Stack                │
              │ Frames               │
              │ Heap                 │
              │ Instructions         │
              └──────────┬───────────┘
                         │
                         ▼
                    Execution
```

The current VM is therefore intended to be the **runtime foundation** rather than the final language implementation.

---

# License

Add the project's chosen license here before distributing the interpreter.

For example:

```text
MIT License
```

or:

```text
Apache License 2.0
```


@@##@@
@@ps: This is based on the original C4 which i don't have rights on and it belongs to the sole owner himself, I had simply created this to understand more abt the functions and possible applications of c, to learn dataflow and more, this variant belongs to me as , i had used some ai to help me out in figuring out the syntax and parsing, This is a project that i worked on being a student to understand the dynamics on c i wish to work on it a bit more and keep developing more and more, please give me suggestions if u have any i am just a student hence if u find anything, which can be improved feel free to give me some advice and tips.
