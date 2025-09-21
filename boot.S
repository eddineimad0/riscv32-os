.globl boot

.extern __stack_top

.section .text.boot
boot:
        mv sp, __stack_top
        j kernel_main

