.section .jumps,"ax",@progbits

.global get_mac
.global write_page_to_flash

get_mac:
    jmp _get_mac

write_page_to_flash:
    jmp _write_page_to_flash
