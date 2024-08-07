/*
	Filename: flash.c
	Description: flash memory read/write API for CH559 512 bytes data flash
*/
#include <stdio.h>
#include "dataflash.h"
#include "ttbasic.h"

extern void c_puts(const char *s);
extern void putnum(short value, short d);

void flash_write(unsigned char *buf, short iSize){
	//CH559
	//for (uint8_t i=0;i<512;i++){
		//eeprom_write_byte(i, *(buf+(i>>1)));
	//	}
	//c_puts("512 words/c_puts("512 words written.\n");	
 
	if (dataflash_write(buf,iSize))
		{c_puts("W:"); putnum(iSize,0);c_puts("\n\r");}
}

void flash_read(unsigned char *buf, short iSize){
	//for (uint8_t i=0;i<1024;i++) {
		//*(buf+i) = eeprom_read_byte(i);
	//	}
	//c_puts("1024 bytes read.\n");
 
	if (dataflash_read(buf, iSize))
		{c_puts("R:"); putnum(iSize,0);c_puts("\n\r");}
	#if _ENABLE_DBG_
		uint16_t i,k;
		char cbuf[4];
  		for (i=0,k=1;i<iSize;i++) {
		   //putnum(*(buf+i),4); 
			if (i==0) c_puts("  00:");
			sprintf(cbuf, " %02X", *(buf+i));
			c_puts(cbuf);
        	if ((k++)>15) {c_puts("\n\r"); k=1; putnum(i+1,4);c_puts(":");}
        	}  
		c_puts("\n\r");
	#endif
}

unsigned char getBootflag(){
	unsigned char bf=dataflash_at(BOOTARG_ADDR); //SIZE_LIST-1
	char buf[5];
	sprintf(buf, "[%02X]", bf);
	//c_puts("BOOTMARK:0x");//putnum(bf,3);
	c_puts(buf);
	return  bf;
}

