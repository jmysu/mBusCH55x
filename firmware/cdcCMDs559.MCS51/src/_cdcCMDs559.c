/*
 *  _cdcTemplate.c
 *
 *    CH55x CDC Template  w/ CDC printf & Serial.printf(UART0)
 *
 *    by jimmy.su @ 2024
 *
 *    LINKFLAGS: -mmcs51 --iram-size 256 --xram-size 6144 --code-size 65536 --out-fmt-ihx --xram-size 6144 --xram-loc 0 --model-large --stack-auto
 *
 *    Flash: [===       ]  29.9% (used 19593 bytes from 65536 bytes) UART Mode
 *    Flash: [====      ]  38.6% (used 25283 bytes from 65536 bytes) CDC Mode
 *
 *    __   __
 *    P3.2 -|o\_/ |- 3.3V (connect to GND via 0.1uF)
 *    P1.4 -|  C  |- 5.0V (connect to USB VBUS, connect to GND via 0.1uF)
 *    P1.5 -|  H  |- GND  (to USB GND)
 *    P1.6 -|  5  |- USB D-
 *    P1.7 -|  5  |- USB D+ (short 3.3v to BOOT)
 *    Reset-|  2  |- P3.4
 *    P3.1 -|  G  |- P3.3
 *    P3.0 -|_____|- P1.1
 *
 *
 *    0801: Flash: [====      ]  41.9% (used 27440 bytes from 65536 bytes)
 *          TinyBasic cdc559-1000
 *    0803: Flash: [====      ]  42.0% (used 27518 bytes from 65536 bytes)
 *          Add isCDCarrived, kbhit
 *    0803b:Flash: [====      ]  42.1% (used 27619 bytes from 65536 bytes)
 *          Fix _BOOTMARK_FE 0xFE
 *    0803c:Flash: [====      ]  42.3% (used 27692 bytes from 65536 bytes)
 *          Add WAIT delay
 *    0803d:Flash: [====      ]  42.3% (used 27722 bytes from 65536 bytes)
 *          Add RESET
 *
 */
#include <stdarg.h>
#include <stdio.h>

// lib/ch559
#include "ch559.h"
#include "gpio.h"
#include "io.h"
#include "serial.h"

#ifdef _ENABLE_CDC_
#include "usb/cdc_device.h"
#endif
// lib/Utils
// #include "ansiColor.h"
#ifdef _TITLE_STAMP_
#warning "W/ TITLE_STAMP"
#include "myDate.h"
#else
#warning "W/O TITLE_STAMP"
#endif

// ToYoShiKi TinyBasic==========================================================
#include "dataflash.h"
#include "ttbasic.h"
extern void basic(void);

#include "myCmds.h"
#ifdef _ENABLE_CDC_
uint8_t cdcBuffer[32];  // cdc buffer
///////////////////////////////////////////////////////////////////////////////
static const char kString01Manufacturer[] = "jimmyCraft";
static const char kString02Product[] = "CDC TTBasic";
static const char kString03SerialNumber[] = "CH559";

static uint8_t get_string_length(uint8_t no) {
    switch (no) {
        case 1:
            return sizeof(kString01Manufacturer) - 1;
        case 2:
            return sizeof(kString02Product) - 1;
        case 3:
            return sizeof(kString03SerialNumber) - 1;
    }
    return 0;
}
static const char* get_string(uint8_t no) {
    switch (no) {
        case 1:
            return kString01Manufacturer;
        case 2:
            return kString02Product;
        case 3:
            return kString03SerialNumber;
    }
    return 0;
}

char iCDCcount = 0;
void recv(const uint8_t* buffer, uint8_t len) {
    // Echo back Serial/UART0
    // Serial.printf("recv: %x, %d\n", *buffer, len);

    // Echo back to CDC
    // cdc_device_send(buffer, len);
#ifdef _ENABLE_DBG_
    // cdcBuffer capture, assume len<64
    Serial.printf("CDC[%d]:", len);  // Seral printf
#endif
    iCDCcount = len;
    char i = 0;
    while (len--) {
        cdcBuffer[i++] = *(buffer++);
#ifdef _ENABLE_DBG_
        Serial.printf("|%x", cdcBuffer[i - 1]);  // Serial printf
#endif
    }
#ifdef _ENABLE_DBG_
    Serial.println("|");
#endif
}
#endif  // CDC

#define printf printf_fast
///////////////////////////////////////////////////////////////////////////////
// Re-direct printf to USB-CDC or UART Serial
//
// ANSI C (as in K & R book) defines a syntax but not the implementation.
// Remember this is "bare iron", no operating system.
// You have to provide your own STDIO drivers and the code for those routines
//
int putchar(int c) {
    if (RESET_KEEP) {  // Normal, putc to USB-CDC
#ifdef _ENABLE_CDC_
        cdc_device_send(&c, 1);
#else
        Serial.putc(c);
#endif
    } else {  // Bootloader, direct to Serial UART
        Serial.putc(c);
    }
    return c;
}
//
// Gets a character from the receive buffer.  If there is no data
// in the receive buffer it will wait until  there is data in the receive buffer.
// If you do not want to wait in an infinit loop, use isCDCavaliable first to
// check if there is data before calling getcharCDCBuffer().
#ifdef _ENABLE_CDC_
int getcharCDCBuffer(void) {
    /*
    if (isCDCarrived) {
        isCDCarrived = 0;  // reset counter
        return (int))*cdcBuffer;
    */

    while (!isCDCarrived) {
    }                    // wait
    isCDCarrived = 0;  // reset buffer
    return (int)*cdcBuffer;
}
#endif

// get ONE char from CDC/UART0 buffer
int getchar(void) {
#ifdef _ENABLE_CDC_
    return getcharCDCBuffer();
#else
    return getcharUART0();
#endif
}

///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////
void main(void) {
    initialize();  // Also init UART, USB, toggling next boot
#ifdef _ENABLE_CDC_
    // CDC Device Defines
    struct cdc_device device;
    device.id_vendor = 0xFFFF;
    device.id_product = 0xcdcd;
    device.bcd_device = 0x0100;
    device.get_string_length = get_string_length;
    device.get_string = get_string;
    device.recv = recv;
    cdc_device_init(&device);
    delay(100);
    stampingTitle("CH559-CDC");
#else
    stampingTitle("CH559-UART");
#endif

// #define TEST_DATAFLASH
#ifdef TEST_DATAFLASH
#undef printf
    printf("writing buffer\n\r");
    uint16_t ac = 64;
    uint8_t a[64];
    for (int16_t i = 0; i < ac; i++) {
        a[i] = i;
    }
    uint16_t i, k;
    for (i = 0, k = 1; i < ac; i++) {
        printf("%02X ", a[i]);
        if ((k++) > 15) {
            printf("\n\r");
            k = 1;
        }
    }
    dataflash_write(a, ac);

    printf("reading buffer\n\r");
    dataflash_read(a, ac);

    // uint16_t i,k;
    for (i = 0, k = 1; i < ac; i++) {
        printf("%02X ", a[i]);
        if ((k++) > 15) {
            printf("\n\r");
            k = 1;
        }
    }
#endif  // TEST_DATAFLASH

    cmdHelp();
    cmdPrompt();

    char cPtr = 0;
    while (1) {
#ifdef _ENABLE_CDC_
        if (isCDCarrived) { //Got Buffered data from USB CDC
            char c = getcharCDCBuffer();  // retrive buffered data
#else                         
                            {
            char c = getcharUART0();
#endif
            if (c == '\n' || c == '\r') {
                cmdBuf[cPtr] = 0;  // terminated
                if (cPtr) {
                    char* cmd = cmdParam(0);    //  Parse cmd
                    if (cmd != NULL) {
                        cmdMatch(cmd);          //  Matching cmd
                        cmdPrompt();
                    }
                    cPtr = 0;  // reset
                }
            } else {
                putchar(c);  // Output received char
                cmdBuf[cPtr] = c;
                if (cPtr < 32) cPtr++;
            }
        }
    }
}