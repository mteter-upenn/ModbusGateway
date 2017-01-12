#ifndef MISCFUNCS_H
#define MISCFUNCS_H

#define SHOW_FREE_MEM 0                                // 1 for print free memory

// reset necessaries
#define CPU_RESTART_ADDR (uint32_t *)0xE000ED0C
#define CPU_RESTART_VAL 0x5FA0004
#define CPU_RESTART (*CPU_RESTART_ADDR = CPU_RESTART_VAL);

#if SHOW_FREE_MEM
extern int __bss_end;
extern void *__brkval;

int getFreeMemory();
#endif

void resetArd(void);

void setConstants(void);
void writeGenSetupFile(void);
void writeMtrSetupFile(void);

#endif // SETCONSTANTS_H
