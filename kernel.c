#include "common.h"

#define m_panic(fmt, ...)                                                      \
  do {                                                                         \
    printf("PANIC: %s:%d, " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);     \
    for (;;) {                                                                 \
      __asm__ __volatile__("nop");                                             \
    }                                                                          \
  } while (0)

typedef struct sbiret_t {
  long error;
  long value;
} SbiRet;

extern uint8_t *__bss_start, *__bss_end, *__stack_top;

SbiRet sbi_call(long arg0, long arg1, long arg2, long arg3, long arg4,
                long arg5, long fid, long eid) {
  register long a0 __asm__("a0") = arg0;
  register long a1 __asm__("a1") = arg1;
  register long a2 __asm__("a2") = arg2;
  register long a3 __asm__("a3") = arg3;
  register long a4 __asm__("a4") = arg4;
  register long a5 __asm__("a5") = arg5;
  register long a6 __asm__("a6") = fid;
  register long a7 __asm__("a7") = eid;

  __asm__ __volatile__("ecall"
                       : "=r"(a0), "=r"(a1)
                       : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5),
                         "r"(a6), "r"(a7)
                       : "memory");

  return (SbiRet){.error = a0, .value = a1};
}

void putchar(char ch) {
  SbiRet result = sbi_call((long)ch, 0, 0, 0, 0, 0, 0, 1);
  if (result.error) {
    m_panic("printing to console failed");
  }
}

void printf(const char *fmt, ...) {
  va_list vargs;
  m_va_start(vargs, fmt);
  while (*fmt) {
    if ('%' == *fmt) {
      fmt += 1;
      switch (*fmt) {
      case 'd': {
        const u8 ASCII_NUM_BASE = '0';
        const s32 number = m_va_arg(vargs, u32);
        u32 value = (u32)number;
        if (number < 0) {
          putchar('-');
          value = (u32)(-number);
        }

        u32 divisor = 1;
        while (value / divisor > 9) {
          divisor *= 10;
        }

        u32 offset = 0;
        u32 printed = 0;
        while (divisor > 0) {
          offset = value / divisor;
          if (offset > 0) {
            putchar(ASCII_NUM_BASE + (u8)(offset));
            value = value - (offset * divisor);
            printed += 1;
          }
          divisor /= 10;
        }

        if (printed == 0) {
          putchar(ASCII_NUM_BASE + (u8)(value));
        }
        break;
      }
      case 'x': {
        const char *HEX_STR = "0123456789abcdef";
        const u32 number = m_va_arg(vargs, u32);
        u8 nibble = 0;
        for (u8 shift = 28; shift > 0; shift -= 4) {
          nibble = (number >> shift) & 0xf;
          if (nibble > 0) {
            putchar(HEX_STR[nibble]);
          }
        }
        nibble = number & 0xf;
        putchar(HEX_STR[nibble]);

        break;
      }
      case 's': {
        const char *s = m_va_arg(vargs, const char *);
        while (*s) {
          putchar(*s);
          s += 1;
        }
        break;
      }
      case '\0':
        putchar('%');
        goto end;
      default:
        // unsupported formatter just put them
        putchar(*fmt);
        break;
      }
    } else {
      putchar(*fmt);
    }

    fmt += 1;
  }
end:
  m_va_end(vargs);
}

void kernel_main(void) {
  const size_t bss_len = (size_t)__bss_end - (size_t)__bss_start;
  memset((void *)__bss_start, 0, bss_len);

  const char *boot_msg = "\n\nHello, World!\n";
  for (size_t i = 0; boot_msg[i] != '\0'; i++) {
    putchar(boot_msg[i]);
  }

  printf("1 + 2 = %d, 0x%x\n", 1 + 2, 0x1234abcd);
  printf("\n\nGoodbye, %s\n", "World?");

  m_panic("Reached end of kernel main");
}
