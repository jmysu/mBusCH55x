/*
 *  cdcCMDs.h
 *
 *    CH55x CDC with simple cmd-line commands!
 * 
 *    by jimmy.su @ 2024
 * 
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "io.h"
#include "gpio.h"
#ifdef _CH55X_
 #include "ch554.h"
 #include "eeprom.h"
#else
 #include "ch559.h"
 #include "dataflash.h"
#endif
#include "ansiColor.h"
#include "ttbasic.h"

extern char cmdBuf[];
extern void cmdMatch(char *s);
extern char* cmdParam(char c);

//forware declaration
extern void basic(void);
extern void cmdHelp(void);
extern void cmdHeap(void);
extern void cmdBL(void);
extern void cmdP1W(void);
extern void cmdP1R(void);
extern void cmdEER(void);
extern void cmdEEW(void);
extern void cmdPrompt(void);
extern void cmdReset(void);




