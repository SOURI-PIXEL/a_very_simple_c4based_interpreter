# Minimalist C Interpreter & VM

A lightweight, educational single-pass virtual machine and C subset interpreter inspired by `c4`[cite: 1]. Designed to explore low-level systems engineering, stack-based execution, and memory management.

## Architectural Highlights

- **Explicit Registers:** Manages execution flow using dedicated internal registers (`pc`, `sp`, `bp`, `ax`)[cite: 1].
- **64-Bit Precision:** Built with redefined modern data types (`long long`) to ensure precise pointer and integer alignment[cite: 1].
- **Disjoint Memory Segments:** Features isolated allocations for text, stack, and data segments to accurately model low-level computer architecture[cite: 1].

## Instruction Set & Execution

The VM runs a streamlined opcode set handling control flow, stack manipulation (`IMM`, `LEA`, `JMP`, `CALL`, `PUSH`), direct memory loading/storing (`LI`, `LC`, `SI`), and core ALU arithmetic[cite: 1].

## Quick Start

Compile and run the interpreter using any standard C compiler:

```bash
gcc -o c4 c4.c
./c4

ps: This is based on the original C4 which i dont have rights on and it belongs to the sole owner himself, I had simply created this to understand more abt the functions and possible applications of c, to learn dataflow and more.
