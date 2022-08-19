#include <stdio.h>
#include "yie.h"

int state = 0;
uint16_t pc = 0;
uint8_t  icode = 0;
uint8_t  ifun = 0;
uint64_t reg[15];
uint64_t rA = 0;
uint64_t rB = 0;
uint64_t valA = 0;
uint64_t valB = 0;
uint64_t valC = 0;
uint64_t valE = 0;
uint64_t valM = 0;
uint64_t valS = 0;
uint16_t valP = 0;
uint8_t  rom[] = {
        0x30, 0xf4, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x80, 0x38, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x0d, 0x00, 0x0d, 0x00, 0x0d, 0x00, 0x00, 0x00,
        0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0x00, 0x00,
        0x00, 0x0b, 0x00, 0x0b, 0x00, 0x0b, 0x00, 0x00,
        0x00, 0xa0, 0x00, 0xa0, 0x00, 0xa0, 0x00, 0x00,
        0x30, 0xf7, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x30, 0xf6, 0x04, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x80, 0x56, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x90, 0x63, 0x00,
        0x62, 0x66, 0x70, 0x83, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x50, 0xa7, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xa0, 0xc0,
        0xf7, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0xc0, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0x74, 0x63, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x90
};

int main() {
    while (1) {
        fetch(rom[pc]);
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
        case op:     { printf("fetch op\n");    rA = rom[pc + 1] & 0xf0 >> 4; rB = rom[pc + 1] & 0x0f; valP = pc + 2; break; }
        case pushq:  { printf("fetch pushq\n"); rA = rom[pc + 1] & 0xf0 >> 4;                          valP = pc + 2; break; }
        case popq:   { printf("fetch popq\n");  rA = rom[pc + 1] & 0xf0 >> 4;                          valP = pc + 2; break; }
        case ret:    { printf("fetch ret\n");                                                          valP = pc + 1; break; }
        case halt:   { printf("fetch halt\n");                                                         valP = pc + 1; break; }
        case nop:    { printf("fetch nop\n");                                                          valP = pc + 1; break; }
        case jxx:    {
            printf("fetch jxx\n");
            valC = eightRomBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case call:   {
            printf("fetch call\n");
            valC = eightRomBytes2bigValue(pc + 1);
            valP = pc + 9;
            break;
        }
        case irmovl: {
            printf("fetch irmovl\n");
            rB   =  rom[pc + 1] & 0x0f;
            valC = eightRomBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case rmmovl: {
            printf("fetch rmmovl\n");
            rA = rom[pc + 1] & 0xf0 >> 4;
            rB = rom[pc + 1] & 0x0f;
            valC = eightRomBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case mrmovl: {
            printf("fetch mrmovl\n");
            rA = rom[pc + 1] & 0xf0 >> 4;
            rB = rom[pc + 1] & 0x0f;
            valC = eightRomBytes2bigValue(pc + 2);
            valP = pc + 10;
            break;
        }
        case cmovxx: {
            printf("fetch cmovxx\n");
            rA = rom[pc + 1] & 0xf0 >> 4;
            rB = rom[pc + 1] & 0x0f;
            valP = pc + 2;
            break;
        }
        default:     { printf("fetch error\n"); state = 1; break; }
    }
}

void decode() {
    switch (icode) {
        case mrmovl: { printf("decode mrmovl\n"); valB = rB;                        break; }
        case cmovxx: { printf("decode cmovxx\n"); valA = rA;                        break; }
        case call:   { printf("decode call\n");   valB = reg[rsp];                  break; }
        case rmmovl: { printf("decode rmmovl\n"); valA = rA;       valB = rB;       break; }
        case op:     { printf("decode op\n");     valA = rA;       valB = rB;       break; }
        case pushq:  { printf("decode pushq\n");  valA = rA;       valB = reg[rsp]; break; }
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
                case addq: { printf("execute addq\n"); valE = valB + valA; break; } // TODO: Set CC
                case subq: { printf("execute subq\n"); valE = valB - valA; break; } // TODO: Set CC
                case andq: { printf("execute andq\n"); valE = valB & valA; break; } // TODO: Set CC
                case xorq: { printf("execute xorq\n"); valE = valB ^ valA; break; } // TODO: Set CC
            }
        }
        case pushq:  { printf("execute pushq\n");  valE = valB - 8;    break; }
        case popq:   { printf("execute popq\n");   valE = valB + 8;    break; }
        case ret:    { printf("execute ret\n");    valE = valB + 8;    break; }
        case halt:   { printf("execute halt\n");   state = 1;          break; }
        case nop:    { printf("execute nop\n");                        break; }
        case jxx:    { printf("execute jxx\n");                        break; } // TODO: Cnd <- Cond(CC, ifun)
        case call:   { printf("execute call\n");   valE = valB - 8;    break; }
        case irmovl: { printf("execute irmovl\n"); valE = valC;        break; }
        case rmmovl: { printf("execute rmmovl\n"); valE = valB + valC; break; }
        case mrmovl: { printf("execute mrmovl\n"); valE = valB + valC; break; }
        case cmovxx: { printf("execute cmovxx\n"); valE = valA;        break; }
    }
}

void readMemory() {
    switch (icode) {
        case pushq:  {
            printf("readMemory pushq\n");
            bigEndian2smailEndian(valE);
            smailValue2eightRomBytes(valS, valE);
            break;
        }
        case popq:   {
            printf("readMemory popq\n");
            valE = eightRomBytes2bigValue(valA);
            break;
        }
        case ret:    {
            printf("readMemory ret\n");
            valM = eightRomBytes2bigValue(valA);
            break;
        }
        case call:   {
            printf("readMemory call\n");
            bigEndian2smailEndian(valE);
            smailValue2eightRomBytes(valS, valE);
            break;
        }
        case rmmovl: {
            printf("readMemory rmmovl\n");
            bigEndian2smailEndian(valE);
            smailValue2eightRomBytes(valS, valE);
            break;
        }
        case mrmovl: {
            printf("readMemory mrmovl\n");
            valM = eightRomBytes2bigValue(valA);
            break;
        }
    }
}

void writeMemory() {
    switch (icode) {
        case op: {
            printf("writeMemory op\n");
            bigEndian2smailEndian(valE);
            reg[rax] = valS;
            break;
        }
        case pushq: {
            printf("writeMemory pushq\n");
            bigEndian2smailEndian(valE);
            reg[rsp] = valS;
            break;
        }
        case popq: {
            printf("writeMemory popq\n");
            bigEndian2smailEndian(valE);
            reg[rsp] = valS;
            bigEndian2smailEndian(valM);
            reg[rax] = valM;
            break;
        }
        case call: {
            printf("writeMemory call\n");
            bigEndian2smailEndian(valE);
            reg[rsp] = valS;
            break;
        }
        case ret: {
            printf("writeMemory ret\n");
            bigEndian2smailEndian(valE);
            reg[rsp] = valS;
            break;
        }
        case cmovxx: {
            printf("writeMemory cmovxx\n");
            bigEndian2smailEndian(valE);
            reg[rax] = valS;
            break;
        }
        case irmovl: {
            printf("writeMemory irmovl\n");
            bigEndian2smailEndian(valE);
            reg[rax] = valS;
            break;
        }
        case mrmovl: {
            printf("writeMemory mrmovl\n");
            bigEndian2smailEndian(valM);
            reg[rax] = valS;
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
            // pc = Cnd ? valC : valP;
            break;
        }
    }
}

uint64_t bigEndian2smailEndian(uint64_t valB) {
    valS = valB << 56 | valB << 48 | valB << 40 | valB << 32 | valB << 24 | valB << 16 | valB << 8 | valB;
}

void smailValue2eightRomBytes(uint64_t valS, uint64_t valE) {
    rom[valE]   = (valS & 0xff00000000000000) >> 56;
    rom[valE+1] = (valS & 0x00ff000000000000) >> 48;
    rom[valE+2] = (valS & 0x0000ff0000000000) >> 40;
    rom[valE+3] = (valS & 0x000000ff00000000) >> 32;
    rom[valE+4] = (valS & 0x00000000ff000000) >> 24;
    rom[valE+5] = (valS & 0x0000000000ff0000) >> 16;
    rom[valE+6] = (valS & 0x000000000000ff00) >> 8 ;
    rom[valE+7] =  valS & 0x00000000000000ff;
}

uint64_t eightRomBytes2bigValue(uint16_t pc) {
    uint64_t val = rom[pc+7] << 56 | rom[pc+6] << 48 | rom[pc+5] << 40 | rom[pc+4] << 32 | rom[pc+3] << 24 | rom[pc+2] << 16 | rom[pc+1] << 8 | rom[pc];
    return val;
}