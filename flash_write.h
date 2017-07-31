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
#ifndef _flash_write_h
#define _flash_write_h 1

/*
 * use 16bit address variable for ATmegas with <= 64K flash
 */
#if defined(RAMPZ)
	typedef uint32_t address_t;
#else
	typedef uint16_t address_t;
#endif

/*
 * Calculate the address where the bootloader starts from FLASHEND and BOOTSIZE
 * (adjust BOOTSIZE below and BOOTLOADER_ADDRESS in Makefile if you want to change the size of the bootloader)
 */
//#define BOOTSIZE 1024
#if FLASHEND > 0x0F000
	#define BOOTSIZE 8192
#else
	#define BOOTSIZE 2048
#endif

#define APP_END  (FLASHEND - BOOTSIZE)

int _write_page_to_flash (address_t address, unsigned int size, unsigned char *p);

#endif /* _flash_write_h */
