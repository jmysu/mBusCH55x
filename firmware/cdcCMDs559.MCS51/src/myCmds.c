/*
 *  myCmds.c
 *
 *    CH55x CDC CMDs
 *
 *    by jimmy.su @ 2024
 *
 */

#include "myCmds.h"
#ifdef _CH55X_
_xdata uint8_t cmdBuf[32];  // CH55x
#else
 uint8_t cmdBuf[32];  // cmd input buffer
#endif

typedef struct {
    const char* cmd;
    const char* helper;
    void (*function)(void);
} cmdDictionary;
//=============================================================================
const cmdDictionary tableCmds[] = {
    {"help", "show this help!", cmdHelp},
    {"reset", "reset me!", cmdReset},
    {"bl", "jump boootloader!", cmdBL},
    {"p1w", "write P1(1~7), p1w <x=<1|0>", cmdP1W},
    {"p1r", "read P1(1~7)", cmdP1R},
    {"eew", "write EE(00~3FF), eew <xx=xx>", cmdEEW},
    {"eer", "read EE", cmdEER},
    {"neo", "neopixel <index>,<r>,<g>,<b>", cmdNeopixel},
    {"basic", "TinyBasic", cmdBasic},
    /*---------------------------------------------------*/
    {"?", "show this help!", cmdHelp}};
//=============================================================================
void cmdHelp(void) {
    char iCmdCount = sizeof(tableCmds) / sizeof(cmdDictionary);

    printf("\ncmd HELP\n");
    for (int i = 0; i < 50; i++) printf("_");
    printf("\n");
    for (int i = 0; i < iCmdCount; i++) {
        printf("\033[" ANSI_YELLOW "m");
        printf("%s", tableCmds[i].cmd);
        printf("\033[0m");
        printf("\t%s\n", tableCmds[i].helper);
    }
    for (int i = 0; i < 50; i++) printf("_");
    printf("\n");
}

//=============================================================================
//
void cmdMatch(char* s) {
    int iCmd;
    int iCmdCount = sizeof(tableCmds) / sizeof(cmdDictionary);
    bool bCmdOK = false;

    // Serial.printf("Matching cmd %s:", sToken.c_str());
    // printf("Matching cmd:%s ",s);
    for (iCmd = 0; iCmd < iCmdCount; iCmd++) {
        if (strcmp(s, tableCmds[iCmd].cmd) == 0) {  // compare s with cmds
            // tableCmds[i].function();
            bCmdOK = true;
            break;
        }
    }
    if (!bCmdOK) {
        // Serial.printf();
        printf("Err: %s no found!\n", s);  // 8541
    } else {                               // Found cmd
        tableCmds[iCmd].function();
    }
}

//
//
void cmdPrompt(void) {
    printf("CMD>");
}

//=============================================================================
/* cmdParam(char c)
 *
 *  copy cmdBuf, search buffer and
 *  return the parameter at c
 *
 */
//
#ifdef _CH55X_
_xdata uint8_t cmdBuf1[32];  // CH55x
#else
 uint8_t cmdBuf1[32];  // CH559
#endif

char* cmdParam(char c) {
    strcpy(cmdBuf1, cmdBuf);  // copy buf

    char idx;
    char* token;
    idx = 0;
    // printf("cmdParam:%s\n", buf1);
    //  walk through other tokens
    token = strtok(cmdBuf1, " =,");
    while (token != NULL) {
        // printf ("[%d]:%s\n", idx, token);
        if (idx == c) break;
        idx++;
        token = strtok(NULL, " =,");
    }
    // printf("cmdParam%d:%s\n", c, token);
    return token;
}


///////////////////////////////////////////////////////////////////////////////
//
//
void cmdBasic(void) {
    printf("!Type RESET to return!\n");
    basic();
}

void cmdReset(void) {
    RST_now();
}

void cmdBL(void) {
    printf("Jumping to BootLoader!");
    // CDC_flush();
    // delay(100);
    BOOT_prepare();  // Prepare for jumping to bootloader
    delay(100);      // Need some delay
    // BOOT_now();         //Jump
    runBootloaderF400();
}

/*
SFR(P1_MOD_OC,	0x92);	// port 1 output mode: 0=push-pull, 1=open-drain
SFR(P1_DIR_PU,	0x93);	// port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=1/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
 */
void cmdP1W(void) {
    int pin, v;

    pin = atoi(cmdParam(1));
    v = atoi(cmdParam(2));

    if ((pin > 0) && (pin < 7)) {
// pin output
#ifdef _CH55X_
        // CH55x
        P1_MOD_OC &= ~(1 << (pin & 7));
        P1_DIR_PU |= (1 << (pin & 7));
#else
        // CH559
        P1_DIR &= ~(1 << (pin & 7));
        P1_PU |= (1 << (pin & 7));
#endif
        P1 &= ~(1 << pin);
        P1 |= (v << pin);
    }
    printf("Pin%d:%d[0x%02X]\n", pin, v, P1);
}
/*
SFR(P1_MOD_OC,	0x92);	// port 1 output mode: 0=push-pull, 1=open-drain
SFR(P1_DIR_PU,	0x93);	// port 1 direction for push-pull or pullup enable for open-drain
// Pn_MOD_OC & Pn_DIR_PU: pin input & output configuration for Pn (n=1/3)
//   0 0:  float input only, without pullup resistance
//   0 1:  push-pull output, strong driving high level and low level
//   1 0:  open-drain output and input without pullup resistance
//   1 1:  quasi-bidirectional (standard 8051 mode), open-drain output and input with pullup resistance, just driving high level strongly for 2 clocks if turning output level from low to high
 */
void cmdP1R(void) {
    printf("P1.7654321\n");
    printf("----------\n");
    printf("P1:");
#ifdef _CH55X_
    // ch55x
    P1_MOD_OC = 0xFF;
    P1_DIR_PU = 0xFF;
#else
    // ch559
    P1_DIR |= 0xFF;
    P1_PU |= 0xFF;
#endif
    for (int i = 7; i > 0; i--) {
        printf("%c", P1 & (1 << i) ? '1' : '0');
    }
    printf("\n");
}

/*
 * cmdEER()
 *
 *  read internal 128 data flash
 */
void cmdEER(void) {
    uint16_t i, k;
    printf("\n\033[" ANSI_CYAN "m");
    #ifdef _CH55X_
      printf("CH55x EE:\n");
    #else
      printf("CH559 EE:\n");
    #endif
    for (i = 0; i < 16; i++) printf("%02X ", i);
    printf("\n");
    printf("\033[0m");
    for (i = 0; i < 16; i++) printf("---");
    printf("\n");

// CH55x
#ifdef _CH55X_
    for (i = 0, k = 1; i < 128; i++) {
        printf("%02X ", eeprom_read_byte(i));
        if ((k++) > 15) {
            printf("\n");
            k = 1;
        }
    }
#else  // CH559
    for (i = 0, k = 1; i < 1024; i++) {
        printf("%02X ", dataflash_at(i));  // 0xF000~0xF3FF
        if ((k++) > 15) {
                printf("\n");
                k = 1;
            }
    }
#endif
    for (i = 0; i < 16; i++) printf("===");
    printf("\n");
}

unsigned int hexToInt(const char* hex) {
    unsigned int result = 0;

    while (*hex) {
        if (*hex > 47 && *hex < 58) /*0123456789*/
            result += (*hex - 48);
        else if (*hex > 64 && *hex < 71) /*ABCDEF*/
            result += (*hex - 55);
        else if (*hex > 96 && *hex < 103) /*abcdef*/
            result += (*hex - 87);

        if (*++hex)
            result <<= 4;
    }

    return result;
}
void cmdEEW(void) {
    uint8_t addr = hexToInt(cmdParam(1));
    uint8_t val = hexToInt(cmdParam(2));

    #ifdef _CH55X_
    // CH55x
     if (addr <128)
         eeprom_write_byte(addr, val);
    #else
        dataflash_write(addr, 1);
    #endif

    printf("CH55x EE[%02X]:%02X\n", addr, val);
}

//uint8_t LEDdata[3];
void cmdNeopixel(void) {
    
    char index;
    uint8_t r,g,b;

    index = atoi(cmdParam(1));
    r = atoi(cmdParam(2));
    g = atoi(cmdParam(3));
    b = atoi(cmdParam(4));
    NEOdata[index][0]=r;
    NEOdata[index][1]=g;
    NEOdata[index][2]=b;
    printf("\n Neo1.7[%d]: %02X|%02X|%02X\n", index,  r,g,b);

    //NEO_init();                 // init NeoPixel
    pinMode(1, 7, OUTPUT);
    digitalWrite(1, 7, 0);delayMicroseconds(256); //300ns reset
    /* test
    NEO_writeColor(128,127,129);
    NEO_writeColor(255,254,253);
    NEO_writeColor(3,2,1);

    delayMicroseconds(256); //latch 300ns
    NEO_writeColor(1,2,3);
    delayMicroseconds(256); //latch 300ns
    */
    //flush out all NEOdata
    for (int i=0;i<NEO_COUNT;i++) {
        NEO_writeColor(NEOdata[i][0],NEOdata[i][1],NEOdata[i][2]);
        }
    //END
    digitalWrite(1, 7, 1);
}