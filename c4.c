#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#define int long long

int token; // current token
char *src, *oldsrc; // source code
int poolsize = 256 * 1024; // size of text/data/stack
int *pc, *sp, *bp; 
int ax, cycle; // virtual machine registers
int *text, *stack, *data; // text/data segment

enum { LEA, IMM, JMP, CALL, JZ, JNZ, ENT, ADJ, LEV, LI, LC, SI, SC, PUSH,
    OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR,
    ADD, SUB, MUL, DIV, MOD,
    OPEN, READ, CLOS,
    PRTF,
    MALC,
    MSET,
    MCMP,
    EXIT
};

int eval()
{
    int op, *tmp;

    // Initislize registers
    pc = (int *)text;  //program counter points of start code
    sp = (int *)((uintptr_t)stack + (uintptr_t)poolsize);  //stack pointer points to the end of stack
    bp = sp;  //base pointer points to the end of stack
    ax = 0;  //accumulator register
    cycle = 0;  //cycle counter
    
    while (1)
    {
      cycle++;
      op = *pc++;  //fetch the next instruction

      switch (op)
      {
        case LEA: ax = (int)(bp + *pc++); break;
        case IMM: ax = *pc++; break;
        case JMP: pc = (int *)(uintptr_t)*pc; break;
        case CALL: *--sp = (int *)(uintptr_t)(pc + 1); pc = (int *)(uintptr_t)*pc; break;
        case JZ: pc = ax ? pc + 1 : (int *)(uintptr_t)*pc; break;
        case JNZ: pc = ax ? (int *)(uintptr_t)*pc : pc + 1; break;
        case ENT: *--sp = (int *)(uintptr_t)bp; bp = sp; sp = sp - *pc++; break;
        case ADJ: sp = sp + *pc++; break;
        case LEV: sp = bp; bp = (int *)(uintptr_t)*sp++; pc = (int *)(uintptr_t)*sp++; break;
        case LI: ax = *(int *)(uintptr_t)ax; break;
        case LC: ax = *(char *)(uintptr_t)ax  ; break;
        case SI: *(int *)(uintptr_t)*sp++ = ax; break;
        case SC: *(char *)(uintptr_t)*sp++ = ax; break;
        case PUSH: *--sp = ax; break;

        // ALU operations
        case OR: ax = *sp++ | ax; break;
        case XOR: ax = *sp++ ^ ax; break;
        case AND: ax = *sp++ & ax; break;
        case EQ: ax = *sp++ == ax; break;
        case NE: ax = *sp++ != ax; break;
        case LT: ax = *sp++ < ax; break;
        case GT: ax = *sp++ > ax; break;
        case LE: ax = *sp++ <= ax; break;
        case GE: ax = *sp++ >= ax; break;

        // Arithmetic operations
        case SHL: ax = *sp++ << ax; break;
        case SHR: ax = *sp++ >> ax; break;
        case ADD: ax = *sp++ + ax; break;
        case SUB: ax = *sp++ - ax; break;
        case MUL: ax = *sp++ * ax; break;
        case DIV: ax = *sp++ / ax; break;
        case MOD: ax = *sp++ % ax; break;  
        
        case PRTF:
        printf("%d", ax);
        break;

        case EXIT:
        return ax;

        default:
        printf("Unknown instruction: %d\n", op);
        return -1; // exit with error



    }
    printf("Cycle: %d, Instruction: %d, AX: %d\n", cycle, op, ax);
    
}
}

// --- main funtion ---
int main(int argc, char **argv)
{
text = (int *)malloc(poolsize);  //allocate memory for text segment
stack = (int *)malloc(poolsize);  //allocate memory for stack segment
data = (int *)malloc(poolsize);    //   allocated memory for data 

if (!text || !stack || !data)
{
    printf("Memory allocation failed!\n");
    return 1;

}

int *prog = text;
*prog++ = IMM; *prog++ = 10; 
*prog++ = PUSH; 
*prog++ = IMM; *prog++ = 20; 
*prog++ = ADD; 
*prog++ = PRTF; 
*prog++ = EXIT;

printf("Starting the virtual machine...\n");
int result = eval();
printf("Virtual machine finished with result: %d\n", result);


free(text);
free(stack);
free(data);

return 0;
}