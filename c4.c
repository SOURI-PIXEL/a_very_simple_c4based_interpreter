```c
/*
 * interpreter.c
 *
 * Standard 64-bit Stack-Based Bytecode Interpreter
 *
 * C standard:
 *     C11
 *
 * Build:
 *     gcc -std=c11 -Wall -Wextra -Wpedantic -O2 interpreter.c -o interpreter
 *
 * Windows / MinGW:
 *     gcc -std=c11 -Wall -Wextra -Wpedantic -O2 interpreter.c -o interpreter.exe
 *
 * This is a bytecode virtual machine. It is designed so that a lexer,
 * parser, AST and bytecode compiler can be added later without having
 * to rewrite the VM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>

/* ============================================================
 * Configuration
 * ============================================================ */

#define VM_STACK_BYTES  (256u * 1024u)
#define VM_HEAP_BYTES   (256u * 1024u)
#define VM_CODE_BYTES   (64u  * 1024u)

#define VM_STACK_VALUES \
    (VM_STACK_BYTES / sizeof(Value))

/* ============================================================
 * VM Value Types
 * ============================================================ */

typedef int64_t  Value;
typedef uint64_t UValue;

/* ============================================================
 * Opcode Definitions
 * ============================================================ */

typedef enum
{
    /* Program */
    OP_HALT = 0,

    /* Constants / stack */
    OP_CONST,
    OP_PUSH,
    OP_POP,
    OP_DUP,

    /* Arithmetic */
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,

    /* Bitwise */
    OP_OR,
    OP_XOR,
    OP_AND,
    OP_SHL,
    OP_SHR,

    /* Comparisons */
    OP_EQ,
    OP_NE,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,

    /* Control flow */
    OP_JMP,
    OP_JZ,
    OP_JNZ,

    /* Functions */
    OP_CALL,
    OP_RET,

    /* Stack frames */
    OP_ENTER,
    OP_LEAVE,
    OP_ADJ,

    /* VM heap */
    OP_MALLOC,
    OP_FREE,

    /* Memory */
    OP_LOAD8,
    OP_LOAD64,
    OP_STORE8,
    OP_STORE64,

    /* Memory utilities */
    OP_MEMSET,
    OP_MEMCMP,

    /* I/O */
    OP_PRINT

} Opcode;

/* ============================================================
 * VM Structure
 * ============================================================ */

typedef struct
{
    /* --------------------------------------------------------
     * Bytecode
     * -------------------------------------------------------- */

    const uint8_t *code;
    size_t code_size;

    /* Instruction pointer */
    size_t ip;

    /* --------------------------------------------------------
     * Operand / call stack
     * -------------------------------------------------------- */

    Value *stack;

    size_t stack_capacity;
    size_t sp;

    /*
     * Frame pointer.
     *
     * A call frame is:

         fp + 0 : return address
         fp + 1 : previous frame pointer
         fp + 2 : local variables / frame data
     */
    size_t fp;

    /* --------------------------------------------------------
     * VM heap
     * -------------------------------------------------------- */

    uint8_t *heap;

    size_t heap_size;
    size_t heap_used;

    /* --------------------------------------------------------
     * Registers / state
     * -------------------------------------------------------- */

    Value ax;

    bool running;

    int exit_code;

    uint64_t cycles;

} VM;

/* ============================================================
 * Bytecode Builder
 * ============================================================ */

typedef struct
{
    uint8_t *data;

    size_t size;
    size_t capacity;

} Bytecode;

/* ============================================================
 * Error Handling
 * ============================================================ */

static void vm_error(VM *vm, const char *message)
{
    fprintf(
        stderr,
        "\nVM ERROR at instruction pointer %zu:\n"
        "  %s\n",
        vm->ip,
        message
    );

    vm->running = false;
    vm->exit_code = EXIT_FAILURE;
}

/* ============================================================
 * VM Initialization
 * ============================================================ */

static bool vm_init(
    VM *vm,
    size_t stack_capacity,
    size_t heap_size
)
{
    memset(vm, 0, sizeof(*vm));

    vm->stack = calloc(
        stack_capacity,
        sizeof(Value)
    );

    if (vm->stack == NULL)
    {
        fprintf(
            stderr,
            "Failed to allocate VM stack.\n"
        );

        return false;
    }

    vm->stack_capacity = stack_capacity;

    vm->heap = calloc(
        heap_size,
        sizeof(uint8_t)
    );

    if (vm->heap == NULL)
    {
        fprintf(
            stderr,
```
