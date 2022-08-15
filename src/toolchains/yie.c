#include <stdio.h>
#include <stdint.h>
#include "yie.h"


int state = 0;
uint8_t word = 0;
uint16_t pc = 0;
uint64_t reg[15];
uint64_t memory[65535];
uint64_t rom[] = {
        0x30f4000200000000,
        0x0000803800000000,
        0x00000000
};

int main() {
    pc = 0;
    while (1) {
        fetch(word);
        decode();
        execute();
        readMemory();
        writeMemory();
        updatePC();
        if (state) break;
    }
    return 0;
}

void fetch() {
    printf("fetchOnce()\n");
}

void decode() {
    printf("decodeOnce()\n");
}

void execute() {
    printf("executeOnce()\n");
}

void readMemory() {
    printf("readMemoryOnce()\n");
}

void writeMemory() {
    printf("writeMemoryOnce()\n");
}

void updatePC() {
    pc += 1;
}