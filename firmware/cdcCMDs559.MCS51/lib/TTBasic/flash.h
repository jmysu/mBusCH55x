/*
	Filename: flash.h
	Description: flash memory read/write API
*/

void flash_write(unsigned char*, short size);
void flash_read(unsigned char*, short size);
unsigned char getBootflag(void);
 