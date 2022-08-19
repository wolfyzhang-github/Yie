#ifndef YIE_H
#define YIE_H

#include <stdint.h>

#define HLT 1
#define ADR 1
#define INS 1
#define AOK 0

void fetch(uint8_t icode_ifun);
void decode();
void execute();
void readMemory();
void writeMemory();
void updatePC();
uint64_t bigEndian2smailEndian(uint64_t val);
void smailValue2eightRomBytes(uint64_t valS, uint64_t valE);
uint64_t eightRomBytes2bigValue(uint16_t pc);

/* icode*/
#define halt   0x0
#define nop    0x1
#define cmovxx 0x2
#define irmovl 0x3
#define rmmovl 0x4
#define mrmovl 0x5
#define op     0x6
#define jxx    0x7
#define call   0x8
#define ret    0x9
#define pushq  0xa
#define popq   0xb


#define addq   0x0
#define subq   0x1
#define andq   0x2
#define xorq   0x3
#define jmp    0x0
#define jle    0x1
#define jl     0x2
#define je     0x3
#define jne    0x4
#define jge    0x5
#define jg     0x6
#define cmovle 0x1
#define cmovl  0x2
#define cmove  0x3
#define cmovne 0x4
#define cmovge 0x5
#define cmovg  0x6

#define rax    0x0
#define rcx    0x1
#define rdx    0x2
#define rsp    0x4
#define rbp    0x5
#define rsi    0x6
#define rdi    0x7
#define r8     0x8
#define r9     0x9
#define r10    0xa
#define r11    0xb
#define r12    0xc
#define r13    0xd
#define r14    0xe

#endif
