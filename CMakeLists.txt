cmake_minimum_required(VERSION 3.28)
project(tetris-os C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang")
set(CMAKE_ASM_COMPILER "/usr/bin/clang")

add_compile_options(
  -Wall -Wextra -Wpedantic -Wformat=2
  --target=riscv32-unknown-elf -fno-stack-protector --frestanding -nostdlib
)

add_link_options( -Tkernel.ld -wL,-Map=kernel.map)

add_executable(kernel.elf kernel.c)
