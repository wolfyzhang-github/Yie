#include <stdio.h>
#include <string.h>
#include "yie.h"

uint16_t pc = 0;
uint8_t  icode, ifun, state = 0;
uint64_t reg[15];
uint64_t rA, rB, valA, valB, valC, valE, valM, valS = 0;
uint16_t valP = 0;
uint8_t  memory[MEM_SIZE];
uint8_t  ZF, SF, OF, Cnd;

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
    for (int i = 0; ; ++i) {
        printf("\n"TITLE"No.%d cycle:"COLOR_NONE"\n", i+1);
        fetch(memory[pc]);
        decode();
        execute();
        readMemory();
        writeMemory();
        updatePC();
        if (i == MEM_SIZE) {
            state = HLT;
            printf("\n"STATE"Suspected dead cycle, please check your program!"COLOR_NONE"\n");
        }
        if (state) break;
    }

    return 0;
}

void fetch(uint8_t icode_ifun) {
    icode = (icode_ifun & 0xf0) >> 4;
    ifun  =  icode_ifun & 0x0f;

    switch (icode) {
        case op: {
            printf(YELLOW"fetch op:\t"COLOR_NONE"    ");
            printf("rA = %lu; rB = %lu; valP = %hu\n", rA, rB, valP);
            rA = (memory[pc + 1] & 0xf0) >> 4;
            rB = (memory[pc + 1]) & 0x0f;
            valP = pc + 2;
            break;
        }
        case pushq: {
            printf(YELLOW"fetch pushq:\t"COLOR_NONE"    ");
            printf("rA = %lu; valP = %hu\n", rA, valP);
            rA = (memory[pc + 1] & 0xf0) >> 4;
            valP = pc + 2;
            break;
        }
        case popq: {
            printf(YELLOW"fetch popq:\t"COLOR_NONE"    ");
            printf("rA = %lu; valP = %hu\n", rA, valP);
            rA = (memory[pc + 1] & 0xf0) >> 4;
            valP = pc + 2;
            break;
        }
        case ret: {
            printf(YELLOW"fetch ret:\t"COLOR_NONE"    ");
            printf("rA = %lu; valP = %hu\n", rA, valP);
            valP = pc + 1;
            break;
        }
        case halt: {
            printf(RED"fetch halt:\t"COLOR_NONE"    ");
            printf("valP = %hu\n", valP);
            valP = pc + 1;
            break;
        }
        case nop: {
            printf(YELLOW"fetch nop:\t"COLOR_NONE"    ");
            printf("valP = %hu\n", valP);
            valP = pc + 1;
            break;
        }
        case jxx: {
            printf(YELLOW"fetch jxx:\t"COLOR_NONE"    ");
            printf("valC = %lu; valP = %hu\n", valC, valP);
            valC = eightRamBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case call:   {
            printf(YELLOW"fetch call:\t"COLOR_NONE"    ");
            printf("valC = %lu; valP = %hu\n", valC, valP);
            valC = eightRamBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case irmovl: {
            printf(YELLOW"fetch irmovl:\t"COLOR_NONE"    ");
            printf("rB = %lu; valC = %lu; valP = %hu\n", rB, valC, valP);
            rB   =  memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case rmmovl: {
            printf(YELLOW"fetch rmmovl:\t"COLOR_NONE"    ");
            printf("rA = %lu; rB = %lu; valC = %lu; valP = %hu\n", rA, rB, valC, valP);
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case mrmovl: {
            printf(YELLOW"fetch mrmovl:\t"COLOR_NONE"    ");
            printf("rA = %lu; rB = %lu; valC = %lu; valP = %hu\n", rA, rB, valC, valP);
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valC = eightRamBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case cmovxx: {
            printf(YELLOW"fetch cmovxx:\t"COLOR_NONE"    ");
            printf("rA = %lu; rB = %lu; valP = %hu\n", rA, rB, valP);
            rA = memory[pc + 1] & 0xf0 >> 4;
            rB = memory[pc + 1] & 0x0f;
            valP = pc + 2;
            break;
        }
        default: { printf(STATE"fetch error! state = INS!"COLOR_NONE"\n"); state = INS; break; }
    }
}

void decode() {
    switch (icode) {
        case mrmovl: {
            valB = reg[rB];
            printf(BLUE"decode mrmovl:\t"COLOR_NONE"    ");
            printf("valB = reg[%lu] = %lu\n", rB, valB);
            break;
        }
        case cmovxx: {
            valA = reg[rA];
            printf(BLUE"decode cmovxx:\t"COLOR_NONE"    ");
            printf("valA = reg[%lu] = %lu\n", rA, valA);
            break;
        }
        case call: {
            valB = reg[rsp];
            printf(BLUE"decode call:\t"COLOR_NONE"    ");
            printf("valB = reg[%d] = %lu\n", rsp, valB);
            break;
        }
        case rmmovl: {
            valA = reg[rA];
            valB = reg[rB];
            printf(BLUE"decode rmmovl:\t"COLOR_NONE"    ");
            printf("valA = reg[%lu] = %lu; valB = reg[%lu] = %lu\n", rA, valA, rB, valB);
            break;
        }
        case op: {
            valA = reg[rA];
            valB = reg[rB];
            printf(BLUE"decode op:\t"COLOR_NONE"    ");
            printf("valA = reg[%lu] = %lu; valB = reg[%lu] = %lu\n", rA, valA, rB, valB);
            break;
        }
        case pushq: {
            valA = reg[rA];
            valB = reg[rsp];
            printf(BLUE"decode pushq:\t"COLOR_NONE"    ");
            printf("valA = reg[%lu] = %lu; valB = reg[%d] = %lu\n", rA, valA, rsp, valB);
            break;
        }
        case popq: {
            valA = reg[rsp];
            valB = reg[rsp];
            printf(BLUE"decode popq:\t"COLOR_NONE"    ");
            printf("valA = reg[%d] = %lu; valB = reg[%d] = %lu\n", rsp, valA, rsp, valB);
            break;
        }
        case ret: {
            valA = reg[rsp];
            valB = reg[rsp];
            printf(BLUE"decode ret:\t"COLOR_NONE"    ");
            printf("valA = reg[%d] = %lu; valB = reg[%d] = %lu\n", rsp, valA, rsp, valB);
            break;
        }
        case halt: {
            printf(RED"decode halt\n"COLOR_NONE);
            break;
        }
        case nop: {
            printf(BLUE"decode nop\n"COLOR_NONE);
            break;
        }
        case jxx: {
            printf(BLUE"decode jxx\n"COLOR_NONE);
            break;
        }
        case irmovl: {
            printf(BLUE"decode irmovl\n"COLOR_NONE);
            break;
        }
        default: { state = INS; printf(STATE"decode error! state = INS!"COLOR_NONE"\n");   break; }
    }
}

void execute() {
    switch (icode) {
        case op: {
            switch (ifun) {
                case addq: {
                    valE = valB + valA;
                    if (valB + valA > 0xffffffffffffffff)   { OF = 1; } else { OF = 0; }
                    if (valB + valA < 0)                    { SF = 1; } else { SF = 0; }
                    if (valB + valA == 0)                   { ZF = 1; } else { ZF = 0; }
                    printf(GREEN"execute addq:\t"COLOR_NONE"    ");
                    printf("valE = %lu; valA = %lu; valB = %lu;", valE, valA, valB);
                    printf(" OF = %d; SF = %d; ZF = %d\n", OF, SF, ZF);
                    break;
                }
                case subq: {
                    valE = valB - valA;
                    if (valB - valA > 0xffffffffffffffff)   { OF = 1; } else { OF = 0; }
                    if (valB - valA < 0)                    { SF = 1; } else { SF = 0; }
                    if (valB - valA == 0)                   { ZF = 1; } else { ZF = 0; }
                    printf(GREEN"execute subq:\t"COLOR_NONE"    ");
                    printf("valE = %lu; valA = %lu; valB = %lu;", valE, valA, valB);
                    printf(" OF = %d; SF = %d; ZF = %d\n", OF, SF, ZF);
                    break;
                }
                case andq: {
                    valE = valB & valA;
                    if ((valB & valA) > 0xffffffffffffffff) { OF = 1; } else { OF = 0; }
                    if ((valB & valA) < 0)                  { SF = 1; } else { SF = 0; }
                    if ((valB & valA) == 0)                 { ZF = 1; } else { ZF = 0; }
                    printf(GREEN"execute andq:\t"COLOR_NONE"    ");
                    printf("valE = %lu; valA = %lu; valB = %lu", valE, valA, valB);
                    printf(" OF = %d; SF = %d; ZF = %d\n", OF, SF, ZF);
                    break;
                }
                case xorq: {
                    valE = valB ^ valA;
                    if ((valB ^ valA) > 0xffffffffffffffff) { OF = 1; } else { OF = 0; }
                    if ((valB ^ valA) < 0)                  { SF = 1; } else { SF = 0; }
                    if ((valB ^ valA) == 0)                 { ZF = 1; } else { ZF = 0; }
                    printf(GREEN"execute xorq:\t"COLOR_NONE"    ");
                    printf("valE = %lu; valA = %lu; valB = %lu", valE, valA, valB);
                    printf(" OF = %d; SF = %d; ZF = %d\n", OF, SF, ZF);
                    break;
                }
            }
            break;
        }
        case pushq: {
            valE = valB - 8;
            printf(GREEN"execute pushq:\t"COLOR_NONE"    ");
            printf("valE = %lu; valB = %lu\n", valE, valB);
            break;
        }
        case popq: {
            valE = valB + 8;
            printf(GREEN"execute popq:\t"COLOR_NONE"    ");
            printf("valE = %lu; valB = %lu\n", valE, valB);
            break;
        }
        case ret: {
            valE = valB + 8;
            printf(GREEN"execute ret\n"COLOR_NONE);
            break;
        }
        case halt: {
            state = HLT;
            printf(RED"execute halt\t"COLOR_NONE"    ");
            printf(STATE"state = HLT"COLOR_NONE"\n");
            break;
        }
        case nop: {
            printf(GREEN"execute nop\n"COLOR_NONE);
            break;
        }
        case jxx: {
            switch (ifun) {
                case jmp: {
                    Cnd = 1;
                    printf(GREEN"execute jmp:\t"COLOR_NONE"    ");
                    printf("Cnd = 1\n");
                    break;
                }
                case jle: {
                    if ((SF ^ OF) || ZF) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute jle:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; ZF = %d; Cnd = %d\n", SF, OF, ZF, Cnd);
                    break;
                }
                case jl: {
                    if (SF ^ OF) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute jl:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; Cnd = %d\n", SF, OF, Cnd);
                    break;
                }
                case jge: {
                    if (!(SF ^ OF)) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute jge:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; Cnd = %d\n", SF, OF, Cnd);
                    break;
                }
                case jg: {
                    if (!(SF ^ OF) & !ZF) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute jg:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; ZF = %d; Cnd = %d\n", SF, OF, ZF, Cnd);
                    break;
                }
                case je: {
                    if (ZF) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute je:\t"COLOR_NONE"    ");
                    printf("ZF = %d; Cnd = %d\n", ZF, Cnd);
                    break;
                }
                case jne: {
                    if (!ZF) {
                        Cnd = 1;
                    } else {
                        Cnd = 0;
                    }
                    printf(GREEN"execute jne:\t"COLOR_NONE"    ");
                    printf("ZF = %d; Cnd = %d\n", ZF, Cnd);
                    break;
                }
            }
            break;
        }
        case call: {
            valE = valB - 8;
            printf(GREEN"execute call:\t"COLOR_NONE"    ");
            printf("valE = %lu; valB = %lu\n", valE, valB);
            break;
        }
        case irmovl: {
            valE = valC;
            printf(GREEN"execute irmovl:\t"COLOR_NONE"    ");
            printf("valE = %lu; valC = %lu\n", valE, valC);
            break;
        }
        case rmmovl: {
            valE = valB + valC;
            printf(GREEN"execute rmmovl:\t"COLOR_NONE"    ");
            printf("valE = %lu; valB = %lu; valC = %lu\n", valE, valB, valC);
            break;
        }
        case mrmovl: {
            valE = valB + valC;
            printf(GREEN"execute mrmovl:\t"COLOR_NONE"    ");
            printf("valE = %lu; valB = %lu; valC = %lu\n", valE, valB, valC);
            break;
        }
        case cmovxx: {
            switch (ifun) {
                case rrmovl: {
                    valE = valA;
                    printf(GREEN"execute rrmovl:\t"COLOR_NONE"    ");
                    printf("valE = %lu; valA = %lu\n", valE, valA);
                    break;
                }
                case cmovle: {
                    if ((SF ^ OF) || ZF) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmovle:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; ZF = %d; valE = %lu; valA = %lu\n", SF, OF, ZF, valE, valA);
                    break;
                }
                case cmovl: {
                    if (SF ^ OF) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmovl:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; valE = %lu; valA = %lu\n", SF, OF, valE, valA);
                    break;
                }
                case cmovge: {
                    if (!(SF ^ OF)) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmovge:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; valE = %lu; valA = %lu\n", SF, OF, valE, valA);
                    break;
                }
                case cmovg: {
                    if (!(SF ^ OF) & !ZF) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmovg:\t"COLOR_NONE"    ");
                    printf("SF = %d; OF = %d; ZF = %d; valE = %lu; valA = %lu\n", SF, OF, ZF, valE, valA);
                    break;
                }
                case cmove: {
                    if (ZF) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmove:\t"COLOR_NONE"    ");
                    printf("ZF = %d; valE = %lu; valA = %lu\n", ZF, valE, valA);
                    break;
                }
                case cmovne: {
                    if (!ZF) {
                        valE = valA;
                    }
                    printf(GREEN"execute cmovne:\t"COLOR_NONE"    ");
                    printf("ZF = %d; valE = %lu; valA = %lu\n", ZF, valE, valA);
                    break;
                }
            }
            break;
        }
    }
}

void readMemory() {
    switch (icode) {
        case pushq: {
            bigEndian2smailEndian(valE);
            smailValue2eightRamBytes(valS, valE);
            printf(PURPLE"readMemory pushq:"COLOR_NONE"   ");
            printf("valE = %lx; valS = %lx\n", valE, valS);
            break;
        }
        case popq: {
            valE = eightRamBytes2bigValue(valA);
            printf(PURPLE"readMemory popq:"COLOR_NONE"    ");
            printf("valE = %lx; valA = %lx\n", valE, valA);
            break;
        }
        case ret: {
            valM = eightRamBytes2bigValue(valA);
            printf(PURPLE"readMemory ret:"COLOR_NONE"     ");
            printf("valM = %lx; valA = %lx\n", valM, valA);
            break;
        }
        case call: {
            bigEndian2smailEndian(valP);
            smailValue2eightRamBytes(valS, valE);
            printf(PURPLE"readMemory call:"COLOR_NONE"    ");
            printf("valP = %hx; valS = %lx valE = %lx\n", valP, valS, valE);
            break;
        }
        case rmmovl: {
            bigEndian2smailEndian(valE);
            smailValue2eightRamBytes(valS, valE);
            printf(PURPLE"readMemory rmmovl:"COLOR_NONE"  ");
            printf("valE = %lx; valS = %lx\n", valE, valS);
            break;
        }
        case mrmovl: {
            valM = eightRamBytes2bigValue(valA);
            printf(PURPLE"readMemory mrmovl:"COLOR_NONE"  ");
            printf("valM = %lx; valA = %lx\n", valM, valA);
            break;
        }
    }
}

void writeMemory() {
    switch (icode) {
        case op: {
            reg[rB] = valE;
            printf(DGREEN"writeMemory op:"COLOR_NONE"     ");
            printf("reg[%lu] = valE = %lu\n", rB, reg[rB]);
            break;
        }
        case pushq: {
            reg[rsp] = valE;
            printf(DGREEN"writeMemory pushq:"COLOR_NONE" ");
            printf("reg[%d] = valE = %lu\n", rsp, reg[rsp]);
            break;
        }
        case popq: {
            reg[rsp] = valE;
            reg[rA] = valM;
            printf(DGREEN"writeMemory popq:"COLOR_NONE"   ");
            printf("reg[%d] = valE = %lu; reg[%lu] = valM = %lu\n", rsp, reg[rsp], rA, reg[rA]);
            break;
        }
        case call: {
            reg[rsp] = valE;
            printf(DGREEN"writeMemory call:"COLOR_NONE"   ");
            printf("reg[%d] = valE = %lu\n", rsp, reg[rsp]);
            break;
        }
        case ret: {
            reg[rsp] = valE;
            printf(DGREEN"writeMemory ret:"COLOR_NONE"    ");
            printf("reg[%d] = valE = %lu\n", rsp, reg[rsp]);
            break;
        }
        case cmovxx: {
            reg[rB] = valE;
            printf(DGREEN"writeMemory cmovxx:"COLOR_NONE" ");
            printf("reg[%lu] = valE = %lu\n", rB, valE);
            break;
        }
        case irmovl: {
            reg[rB] = valE;
            printf(DGREEN"writeMemory irmovl:"COLOR_NONE" ");
            printf("reg[%lu] = valE = %lu\n", rB, valE);
            break;
        }
        case mrmovl: {
            reg[rA] = valM;
            printf(DGREEN"writeMemory mrmovl:"COLOR_NONE" ");
            printf("reg[%lu] = valM = %lu\n", rA, valM);
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
            pc = valP;
            printf(BOLD"updatePC:\t"COLOR_NONE"    ");
            printf("pc = valP = %d\n", valP);
            break;
        }
        case call: {
            pc = valC;
            printf(BOLD"updatePC:\t"COLOR_NONE"    ");
            printf("pc = valC = %lu\n", valC);
            break;
        }
        case ret: {
            pc = valM;
            printf(BOLD"updatePC:\t"COLOR_NONE"    ");
            printf("pc = valM = %lu\n", valM);
            break;
        }
        case jxx: {
            pc = Cnd ? valC : valP;
            printf(BOLD"updatePC:\t"COLOR_NONE"    ");
            printf("pc = Cnd ? valC : valP = %hu\n", pc);
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