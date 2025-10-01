.file "boot.S"
.option norvc # disables RVC compressed instructions

.globl boot
.extern __stack_top

.section .text.boot,"ax",@progbits
boot:
        la sp, __stack_top
        j kernel_main

