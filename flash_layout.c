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

#include <stddef.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "flash_layout.h"
#include "flash_write.h"

uint32_t _get_part_count ()
{
  return PART_COUNT;
}

uint32_t _get_part_size ()
{
  return (PART_SIZE - PART_IRQVEC_SIZE);
}

uint32_t _get_part_start (uint32_t part_index)
{
  return PART_START (part_index);
}

static void _set_part_ok_ (uint32_t part_index, uint32_t value)
{
  struct flash_directory_s fd;

  if (part_index == _get_active_part ()) {
    fd_read_directory (&fd);
    fd.part_ok [part_index] = value;
    fd_write_directory (&fd);
  }
}

/*
 * Get address of the reset vector (vector 0)
 * We parse an AVR jmp instruction
 * This consists of the following bits according to AVR assembler docs:
 * 1001 010k kkkk 110k kkkk kkkk kkkk kkkk
 * where k are the jump address bits. The other bits are fixed.
 * Since this is a *word* address we shift it one bit to the left.
 * From this address we determine the partition to which the reset
 * vector points.
 */
uint32_t _get_active_part ()
{
  uint32_t jmp =
    (((uint32_t)pgm_read_word_near (0)) << 16) + pgm_read_word_near (2);
  uint32_t adr = ((jmp & 0x1FFFF) + ((jmp & 0x1F00000) >> 3)) << 1;
  return (adr - PART_IMAGES_START) / PART_SIZE;
}

void _set_part_ok (uint32_t part_index)
{
  _set_part_ok_ (part_index, 1);
}

void _clr_part_ok (uint32_t part_index)
{
  _set_part_ok_ (part_index, 0);
}

void _set_boot_default (uint32_t part_index)
{
  struct flash_directory_s fd;

  fd_read_directory (&fd);

  if ((part_index < PART_COUNT) && (fd.part_ok [part_index] == 1)) {
    fd.boot_default = part_index;
    fd.boot_next    = part_index;
    fd_write_directory (&fd);
  }
}

uint32_t _get_boot_default ()
{
  struct flash_directory_s fd;

  fd_read_directory (&fd);

  return fd.boot_default;
}

void _set_boot_next (uint32_t part_index)
{
  struct flash_directory_s fd;

  if (part_index < PART_COUNT) {
    fd_read_directory (&fd);
    fd.boot_next = part_index;
    fd_write_directory (&fd);
  }
}

uint32_t _get_boot_next ()
{
  struct flash_directory_s fd;

  fd_read_directory (&fd);

  return fd.boot_next;
}

void _backup_irq_table (uint32_t part_index)
{
  address_t src_address;
  unsigned char data [SPM_PAGESIZE];
  uint16_t p_count, b_count;

  src_address = _get_part_start (part_index);

  for (p_count = 0; p_count < (PART_IRQVEC_SIZE / SPM_PAGESIZE); p_count ++) {
    for (b_count = 0; b_count < SPM_PAGESIZE; b_count ++) {
      data [b_count] = pgm_read_byte_far (src_address++);
    }
    _write_page_to_flash
      ( PART_IRQVEC_BACKUP_START(part_index) + SPM_PAGESIZE * p_count
      , SPM_PAGESIZE
      , data
      );
  }
}

void update_irq_table (uint32_t part_index)
{
  uint8_t is_equal = 1;
  uint16_t cnt, p_count, b_count;
  address_t src_address, dst_address;
  unsigned char data [SPM_PAGESIZE];

  src_address = PART_IRQVEC_BACKUP_START(part_index);
  dst_address = ABS_IRQVEC_START;

  for (cnt = 0; cnt < PART_IRQVEC_SIZE; cnt++) {
    if (pgm_read_byte_far (src_address++) != pgm_read_byte_far (dst_address++)) {
      is_equal = 0;
      break;
    }
  }

  if (is_equal != 1) {
    src_address = PART_IRQVEC_BACKUP_START(part_index);
    dst_address = ABS_IRQVEC_START;

    for (p_count = 0; p_count < (PART_IRQVEC_SIZE / SPM_PAGESIZE); p_count ++) {
      for (b_count = 0; b_count < SPM_PAGESIZE; b_count ++) {
        data [b_count] = pgm_read_byte_far (src_address++);
      }
      _write_page_to_flash
        ( dst_address + SPM_PAGESIZE * p_count
        , SPM_PAGESIZE
        , data
        );
    }
  }
}

static int fd_get_current_directory (void)
{
  uint32_t magic_0, magic_1;
  uint32_t version_0, version_1;
  uint32_t change_count_0, change_count_1;
  uint32_t valid_0, valid_1;

  magic_0 = pgm_read_dword_far
    (PART_FLASH_DIR_0_START + offsetof (struct flash_directory_s, magic));
  magic_1 = pgm_read_dword_far
    (PART_FLASH_DIR_1_START + offsetof (struct flash_directory_s, magic));

  version_0 = pgm_read_dword_far
    (PART_FLASH_DIR_0_START + offsetof (struct flash_directory_s, version));
  version_1 = pgm_read_dword_far
    (PART_FLASH_DIR_1_START + offsetof (struct flash_directory_s, version));

  change_count_0 = pgm_read_dword_far
    (PART_FLASH_DIR_0_START + offsetof (struct flash_directory_s, change_count));
  change_count_1 = pgm_read_dword_far
    (PART_FLASH_DIR_1_START + offsetof (struct flash_directory_s, change_count));

  valid_0 = (magic_0 == PART_FLASH_DIR_MAGIC) && (version_0 == PART_FLASH_DIR_VERSION);
  valid_1 = (magic_1 == PART_FLASH_DIR_MAGIC) && (version_1 == PART_FLASH_DIR_VERSION);

  if (valid_0 && valid_1) {
    if (change_count_0 > change_count_1) {
      return 0;
    } else {
      return 1;
    }
  } else if (valid_0) {
    return 0;
  } else if (valid_1) {
    return 1;
  } else {
    return -1;
  }
}

void fd_read_directory (struct flash_directory_s *result)
{
  int current_dir = fd_get_current_directory ();
  uint32_t start_address;
  uint32_t count;
  uint8_t * result_ptr = (uint8_t *) result;

  if (current_dir == -1) {
    memset (result, 0, sizeof (*result));
    result->magic        = PART_FLASH_DIR_MAGIC;
    result->version      = PART_FLASH_DIR_VERSION;
    result->change_count = 0;
    result->boot_default = 0;
    result->boot_next    = 0;
    return;
  }

  if (current_dir == 0) {
    start_address = PART_FLASH_DIR_0_START;
  } else if (current_dir == 1) {
    start_address = PART_FLASH_DIR_1_START;
  }

  for (count = 0; count < sizeof (struct flash_directory_s); count ++) {
    *result_ptr++ = pgm_read_byte_far (start_address++);
  }
}

void fd_write_directory (struct flash_directory_s *target) {
  int current_dir = fd_get_current_directory ();
  address_t dest_address;

  target->change_count ++;

  if (current_dir == 0)
    dest_address = PART_FLASH_DIR_1_START;
  else
    dest_address = PART_FLASH_DIR_0_START;

  _write_page_to_flash (dest_address, sizeof (*target), (uint8_t *) target);

}
