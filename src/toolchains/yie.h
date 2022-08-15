#ifndef YIE_H
#define YIE_H

#define HLT 1
#define ADR 1
#define INS 1
#define AOK 0

void fetch();
void decode();
void execute();
void readMemory();
void writeMemory();
void updatePC();

#endif
