.section .jumps,"ax",@progbits

.global get_mac
.global write_page_to_flash
.global get_part_count
.global get_part_size
.global get_part_start
.global get_part_ok
.global set_part_ok
.global clr_part_ok
.global set_boot_default
.global set_boot_next
.global get_boot_default
.global get_boot_next
.global get_active_part

get_mac:
    jmp _get_mac

write_page_to_flash:
    jmp _write_page_to_flash

get_part_count:
    jmp _get_part_count

get_part_size:
    jmp _get_part_size

get_part_start:
    jmp _get_part_start

set_part_ok:
    jmp _set_part_ok

clr_part_ok:
    jmp _clr_part_ok

set_boot_default:
    jmp _set_boot_default

set_boot_next:
    jmp _set_boot_next

get_part_ok:
    jmp _get_part_ok

get_boot_default:
    jmp _get_boot_default

get_boot_next:
    jmp _get_boot_next

get_active_part:
    jmp _get_active_part
