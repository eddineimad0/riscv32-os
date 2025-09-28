# set quiet := true

alias bk:= build-kernel

QEMU := "qemu-system-riscv32"
BUILD_DIR := "./build"
OUTPUT_DIR := "./out"

CC := "/usr/bin/clang"
CFLAGS := "-std=c17 -O2 -g3 -Wall -Wextra -Wpedantic --target=riscv32-unknown-elf -fno-stack-protector -ffreestanding -nostdlib"
LFLAGS := "-Wl,-Tkernel.ld -Wl,-Map=out/kernel.map"


default:
	just --list --unsorted --justfile {{justfile()}} --list-prefix "[+] "

run: build-kernel
	{{QEMU}} -machine virt -bios default -nographic -serial mon:stdio --no-reboot -kernel {{OUTPUT_DIR}}/kernel.elf


build: create_build_dir
	cmake -B {{BUILD_DIR}} -S .
	cmake --build {{BUILD_DIR}}

build-kernel: create_build_dir create_output_dir
	{{CC}} {{CFLAGS}} {{LFLAGS}} -o {{OUTPUT_DIR}}/kernel.elf kernel.c

create_build_dir:
	{{ if path_exists(BUILD_DIR) == "true" {""} else {"mkdir -p " + BUILD_DIR} }}

create_output_dir:
	{{ if path_exists(OUTPUT_DIR) == "true" {""} else {"mkdir -p " + OUTPUT_DIR} }}

