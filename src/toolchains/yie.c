#include <stdio.h>
#include <string.h>
#include "yie.h"

uint16_t pc = 0;
uint8_t  icode, ifun, state = 0;
uint64_t reg[15];
uint64_t rA, rB, valA, valB, valC, valE, valM, valS = 0;
uint16_t valP = 0;
uint8_t  memory[MEM_SIZE];

int main(int argc, char *argv[]) {
    FILE *fp = fopen(argv[1],"r");

    if (fp == NULL) {
        printf("No such file!\n");
        return -1;
    }

    fgets (memory, MEM_SIZE, fp);

    for (int i = 0; i < MEM_SIZE; ++i) {
        if ('0' <= memory[i] && memory[i] <= '9')
            memory[i] = memory[i] - '0';
        else if ('a' <= memory[i] && memory[i] <= 'f')
            memory[i] = memory[i] - 'a' + 10;
    }

    for (int i = 0; i < MEM_SIZE; i = i + 2) {
        memory[i] = memory[i] << 4;
        memory[i] = memory[i] | memory[i + 1];
    }

    unsigned sign = 0;
    for (int i = 0; i < MEM_SIZE; i++) {
        if ((i % 2) == 0) {
            memory[sign] = memory[i];
            sign++;
        }
    }

    fclose(fp);

    pc = 0;
    while (1) {
        fetch(memory[pc]);
        decode();
        execute();
        readMemory();
        writeMemory();
        updatePC();
        if (state) break;
    }

    return 0;
}

void fetch(uint8_t icode_ifun) {
    icode = (icode_ifun & 0xf0) >> 4;
    ifun  =  icode_ifun & 0x0f;

    switch (icode) {
        case op:     { printf("fetch op\n");    rA = memory[pc + 1] & 0xf0 >> 4; rB = memory[pc + 1] & 0x0f; valP = pc + 2; break; }
        case pushq:  { printf("fetch pushq\n"); rA = memory[pc + 1] & 0xf0 >> 4;                          valP = pc + 2; break; }
        case popq:   { printf("fetch popq\n");  rA = memory[pc + 1] & 0xf0 >> 4;                          valP = pc + 2; break; }
        case ret:    { printf("fetch ret\n");                                                          valP = pc + 1; break; }
        case halt:   { printf("fetch halt\n");                                                         valP = pc + 1; break; }
        case nop:    { printf("fetch nop\n");                                                          valP = pc + 1; break; }
        case jxx:    {
            printf("fetch jxx\n");
            valC = eightRamBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case call:   {
            printf("fetch call\n");
            valC = eightRamBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case irmovl: {
            printf("fetch irmovl\n");
            rB   =  memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case rmmovl: {
            printf("fetch rmmovl\n");
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case mrmovl: {
            printf("fetch mrmovl\n");
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case cmovxx: {
            printf("fetch cmovxx\n");
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valP = pc + 2;
            break;
        }
        default:     { printf("fetch error\n"); state = 1; break; }
    }
}

void decode() {
    switch (icode) {
        case mrmovl: { printf("decode mrmovl\n"); valB = reg[rB];                   break; }
        case cmovxx: { printf("decode cmovxx\n"); valA = reg[rA];                   break; }
        case call:   { printf("decode call\n");   valB = reg[rsp];                  break; }
        case rmmovl: { printf("decode rmmovl\n"); valA = reg[rA];  valB = reg[rB];  break; }
        case op:     { printf("decode op\n");     valA = reg[rA];  valB = reg[rB];  break; }
        case pushq:  { printf("decode pushq\n");  valA = reg[rA];  valB = reg[rsp]; break; }
        case popq:   { printf("decode popq\n");   valA = reg[rsp]; valB = reg[rsp]; break; }
        case ret:    { printf("decode ret\n");    valA = reg[rsp]; valB = reg[rsp]; break; }
        case halt:   { printf("decode halt\n");                                     break; }
        case nop:    { printf("decode nop\n");                                      break; }
        case jxx:    { printf("decode jxx\n");                                      break; }
        case irmovl: { printf("decode irmovl\n");                                   break; }
        default:     { printf("decode error\n");  state = 1;                        break; }
    }
}

void execute() {
    switch (icode) {
        case op: {
            switch (ifun) {
                case addq: {
                    printf("execute addq\n");
                    valE = valB + valA;
                    if (valB + valA > 0xffffffffffffffff) {
                        printf("overflow\n");
                        reg[OF] = 1;
                    } else if (valB + valA < 0) {
                        printf("underflow\n");
                        reg[SF] = 1;
                    } else if (valB + valA == 0) {
                        printf("zero\n");
                        reg[ZF] = 1;
                    }
                    break;
                }
                case subq: {
                    printf("execute subq\n");
                    valE = valB - valA;
                    if (valB - valA > 0xffffffffffffffff) {
                        printf("overflow\n");
                        reg[OF] = 1;
                    } else if (valB - valA < 0) {
                        printf("underflow\n");
                        reg[SF] = 1;
                    } else if (valB - valA == 0) {
                        printf("zero\n");
                        reg[ZF] = 1;
                    }
                    break;
                }
                case andq: {
                    printf("execute andq\n");
                    valE = valB & valA;
                    if ((valB & valA) > 0xffffffffffffffff) {
                        printf("overflow\n");
                        reg[OF] = 1;
                    } else if ((valB & valA) < 0) {
                        printf("underflow\n");
                        reg[SF] = 1;
                    } else if ((valB & valA) == 0) {
                        printf("zero\n");
                        reg[ZF] = 1;
                    }
                    break;
                }
                case xorq: {
                    printf("execute xorq\n");
                    valE = valB ^ valA;
                    if ((valB ^ valA) > 0xffffffffffffffff) {
                        printf("overflow\n");
                        reg[OF] = 1;
                    } else if ((valB ^ valA) < 0) {
                        printf("underflow\n");
                        reg[SF] = 1;
                    } else if ((valB ^ valA) == 0) {
                        printf("zero\n");
                        reg[ZF] = 1;
                    }
                    break;
                }
            }
            break;
        }
        case pushq:  { printf("execute pushq\n");  valE = valB - 8;    break; }
        case popq:   { printf("execute popq\n");   valE = valB + 8;    break; }
        case ret:    { printf("execute ret\n");    valE = valB + 8;    break; }
        case halt:   { printf("execute halt\n");   state = 1;          break; }
        case nop:    { printf("execute nop\n");                        break; }
        case jxx:    {
            switch (ifun) {
                case jmp: { printf("execute jmp\n"); reg[Cnd] = 1; break; }
                case jle: { printf("execute jle\n");
                    if ((reg[SF] ^ reg[OF]) || reg[ZF]) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
                case jl: {
                    printf("execute jl\n");
                    if (reg[SF] ^ reg[OF]) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
                case jge: {
                    printf("execute jge\n");
                    if (~(reg[SF] ^ reg[OF])) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
                case jg: {
                    printf("execute jg\n");
                    if (!(reg[SF] ^ reg[OF]) & !reg[ZF]) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
                case je: {
                    printf("execute je\n");
                    if (reg[ZF]) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
                case jne: {
                    printf("execute jne\n");
                    if (!reg[ZF]) {
                        reg[Cnd] = 1;
                    }
                    break;
                }
            }
            break;
        }
        case call:   { printf("execute call\n");   valE = valB - 8;    break; }
        case irmovl: { printf("execute irmovl\n"); valE = valC;        break; }
        case rmmovl: { printf("execute rmmovl\n"); valE = valB + valC; break; }
        case mrmovl: { printf("execute mrmovl\n"); valE = valB + valC; break; }
        case cmovxx: {
            switch (ifun) {
                case rrmovl: { printf("execute rrmovl\n"); valE = valA; ; break; }
                case cmovle: { printf("execute cmovle\n");
                    if ((reg[SF] ^ reg[OF]) || reg[ZF]) {
                        valE = valA;
                    }
                    break;
                }
                case cmovl: {
                    printf("execute cmovl\n");
                    if (reg[SF] ^ reg[OF]) {
                        valE = valA;
                    }
                    break;
                }
                case cmovge: {
                    printf("execute cmovge\n");
                    if (~(reg[SF] ^ reg[OF])) {
                        valE = valA;
                    }
                    break;
                }
                case cmovg: {
                    printf("execute cmovg\n");
                    if (!(reg[SF] ^ reg[OF]) & !reg[ZF]) {
                        valE = valA;
                    }
                    break;
                }
                case cmove: {
                    printf("execute cmove\n");
                    if (reg[ZF]) {
                        valE = valA;
                    }
                    break;
                }
                case cmovne: {
                    printf("execute cmovne\n");
                    if (!reg[ZF]) {
                        valE = valA;
                    }
                    break;
                }
            }
            break;
        }
    }
}

void readMemory() {
    switch (icode) {
        case pushq:  {
            printf("readMemory pushq\n");
            bigEndian2smailEndian(valE);
            smailValue2eightRamBytes(valS, valE);
            break;
        }
        case popq:   {
            printf("readMemory popq\n");
            valE = eightRamBytes2bigValue(valA);
            break;
        }
        case ret:    {
            printf("readMemory ret\n");
            valM = eightRamBytes2bigValue(valA);
            break;
        }
        case call:   {
            printf("readMemory call\n");
            bigEndian2smailEndian(valP);
            smailValue2eightRamBytes(valS, valE);
            break;
        }
        case rmmovl: {
            printf("readMemory rmmovl\n");
            bigEndian2smailEndian(valE);
            smailValue2eightRamBytes(valS, valE);
            break;
        }
        case mrmovl: {
            printf("readMemory mrmovl\n");
            valM = eightRamBytes2bigValue(valA);
            break;
        }
    }
}

void writeMemory() {
    switch (icode) {
        case op: {
            printf("writeMemory op\n");
            reg[rB] = valE;
            break;
        }
        case pushq: {
            printf("writeMemory pushq\n");
            reg[rsp] = valE;
            break;
        }
        case popq: {
            printf("writeMemory popq\n");
            reg[rsp] = valE;
            reg[rA] = valM;
            break;
        }
        case call: {
            printf("writeMemory call\n");
            reg[rsp] = valE;
            break;
        }
        case ret: {
            printf("writeMemory ret\n");
            reg[rsp] = valE;
            break;
        }
        case cmovxx: {
            printf("writeMemory cmovxx\n");
            reg[rB] = valE;
            break;
        }
        case irmovl: {
            printf("writeMemory irmovl\n");
            reg[rB] = valE;
            break;
        }
        case mrmovl: {
            printf("writeMemory mrmovl\n");
            reg[rA] = valM;
            break;
        }
    }
}

void updatePC() {
    switch (icode) {
        case op:
        case irmovl:
        case rmmovl:
        case mrmovl:
        case pushq:
        case popq:
        case cmovxx: {
            printf("updatePC, pc = valP\n");
            pc = valP;
            break;
        }
        case call: {
            printf("updatePC, pc = valC\n");
            pc = valC;
            break;
        }
        case ret: {
            printf("updatePC, pc = valM\n");
            pc = valM;
            break;
        }
        case jxx: {
            printf("updatePC, pc = Cnd ? valC : valP\n");
            pc = reg[Cnd] ? valC : valP;
            break;
        }
    }
}

void bigEndian2smailEndian(uint64_t valB) {
    valS = valB << 56 & 0xFF00000000000000 |
           valB << 40 & 0x00FF000000000000 |
           valB << 24 & 0x0000FF0000000000 |
           valB << 8  & 0x000000FF00000000 |
           valB >> 8  & 0x00000000FF000000 |
           valB >> 24 & 0x0000000000FF0000 |
           valB >> 40 & 0x000000000000FF00 |
           valB >> 56 & 0x00000000000000FF;
}

void smailValue2eightRamBytes(uint64_t valS, uint64_t valE) {
    memory[valE]   = (valS & 0xff00000000000000) >> 56;
    memory[valE+1] = (valS & 0x00ff000000000000) >> 48;
    memory[valE+2] = (valS & 0x0000ff0000000000) >> 40;
    memory[valE+3] = (valS & 0x000000ff00000000) >> 32;
    memory[valE+4] = (valS & 0x00000000ff000000) >> 24;
    memory[valE+5] = (valS & 0x0000000000ff0000) >> 16;
    memory[valE+6] = (valS & 0x000000000000ff00) >> 8 ;
    memory[valE+7] =  valS & 0x00000000000000ff;
}

uint64_t eightRamBytes2bigValue(uint16_t pc) {
    uint64_t val = memory[pc+7] << 56 | memory[pc+6] << 48 | memory[pc+5] << 40 | memory[pc+4] << 32 | memory[pc+3] << 24 | memory[pc+2] << 16 | memory[pc+1] << 8 | memory[pc];
    return val;
}
