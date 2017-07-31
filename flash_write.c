/*
** Copyright (C) 2017 Marcus Priesch, Ralf Schlatterbeck, All rights reserved
** office@priesch.co.at, rsc@runtux.com
** License:   GNU General Public License
**
**++
** Name
**    flash_write
**
** Purpose
**    write a flash page
**
**
** Revision Dates
**    31-Jul-2017 (MPR+RSC) Creation
**    ««revision-date»»···
**--
*/
#include <avr/boot.h>
#include "flash_write.h"

/* MPR: factored write page to flash routine */
int _write_page_to_flash (address_t address, unsigned int size, unsigned char *p) {
	unsigned int	data;
	unsigned char	highByte, lowByte;
	address_t	tempaddress	=	address;

	// erase only main section (bootloader protection)
	if (address + size > APP_END)
		return -1 ;

	if (size > SPM_PAGESIZE)
		return -2 ;

	boot_page_erase(address);	// Perform page erase
	boot_spm_busy_wait();		// Wait until the memory is erased.

	/* Write FLASH */
	do {
		lowByte		=	*p++;
		highByte 	=	*p++;

		data		=	(highByte << 8) | lowByte;
		boot_page_fill(address,data);

		address	=	address + 2;	// Select next word in memory
		size	-=	2;				// Reduce number of bytes to write by two
	} while (size);					// Loop until all bytes written

	boot_page_write(tempaddress);
	boot_spm_busy_wait();
	boot_rww_enable();				// Re-enable the RWW section

	return 0;
}

