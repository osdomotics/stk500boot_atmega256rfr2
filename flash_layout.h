/*
** Copyright (C) 2017 Marcus Priesch, Ralf Schlatterbeck, All rights reserved
** office@priesch.co.at, rsc@runtux.com
** License:   GNU General Public License
** ****************************************************************************
**
**++
** Name
**    flash_layout
**
** Purpose
**    access and switch multiple partitions with executable code
**
** Revision Dates
**    31-Jul-2017 (MPR+RSC) Creation
**    ««revision-date»»···
**--
*/

#ifndef _flash_layout_h
#define _flash_layout_h 1

#include <avr/io.h>

#define PART_COUNT 2

#define PART_SIZE 0x1ef00L

#define PART_IRQVEC_SIZE 0x200
#define PART_IRQVEC_START 0x0

#define PART_IMAGES_START 0x0

#define PART_START(n) (PART_IMAGES_START + (PART_SIZE * n))
#define PART_IRQVEC_BACKUP_START(n) \
            (PART_START(n) + PART_SIZE - PART_IRQVEC_SIZE)

#define PART_FLASH_DIR_SIZE 0x100
#define PART_FLASH_DIR_VERSION 1
#define PART_FLASH_DIR_0_START (PART_START(PART_COUNT-1) + PART_SIZE)
#define PART_FLASH_DIR_1_START (PART_FLASH_DIR_0_START + PART_FLASH_DIR_SIZE)
#define PART_FLASH_DIR_MAGIC 0x08154711L

/* determine address of the caller from the stack */
#define CALLERS_PART \
    ( ( ( (((uint32_t) (*(((uint8_t *) SP) + 3))) << 16L) \
        + (((uint32_t) (*(((uint8_t *) SP) + 4))) <<  8L) \
        + (((uint32_t) (*(((uint8_t *) SP) + 5)))       ) \
        )                                                 \
      - PART_IMAGES_START                                 \
      )                                                   \
    / PART_SIZE                                           \
    )

struct flash_directory_s {
    uint32_t magic;
    uint32_t version;
    uint32_t change_count;
    uint32_t image_size [PART_COUNT];
    uint32_t image_cs   [PART_COUNT];
    uint32_t part_ok    [PART_COUNT];
    uint32_t boot_default;
    uint32_t boot_next;
};

uint32_t _get_part_count   (void);
uint32_t _get_part_size    (void);
uint32_t _get_part_start   (uint32_t part_index);
void     _set_part_ok      (uint32_t part_index);
void     _clr_part_ok      (uint32_t part_index);
void     _set_boot_default (uint32_t part_index);
void     _set_boot_next    (uint32_t part_index);

void fd_read_directory  (struct flash_directory_s *result);
void fd_write_directory (struct flash_directory_s *target);

#endif
