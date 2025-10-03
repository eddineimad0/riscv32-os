#include "common.h"

#define m_panic(fmt, ...)                                                      \
  do {                                                                         \
    printf("PANIC: %s:%d, " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
    for (;;) {                                                                 \
      __asm__ __volatile__("nop");                                             \
    }                                                                          \
  } while (0)

typedef struct sbiret_t {
  long error;
  long value;
} SbiRet;

extern uint8_t *__bss_start, *__bss_end, *__stack_top;
extern void kernel_trap_trampoline(void);

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

  SbiRet result;
  result.error = a0;
  result.value = a1;
  return result;
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
          value = -value;
        }

        u32 divisor = 1;
        while (value / divisor > 9) {
          divisor *= 10;
        }

        u32 offset = 0;
        while (divisor > 0) {
          offset = value / divisor;
            putchar(ASCII_NUM_BASE + (u8)(offset));
            value = value - (offset * divisor);
          divisor /= 10;
        }

        break;
      }
      case 'x': {
        const char *HEX_STR = "0123456789abcdef";
        const u32 number = m_va_arg(vargs, u32);
        u8 nibble = 0;
        for (u8 shift = 28; shift > 0; shift -= 4) {
          nibble = (number >> shift) & 0xf;
            putchar(HEX_STR[nibble]);
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

typedef struct trap_frame_t {
  u32 ra;
  u32 gp;
  u32 tp;
  u32 t0;
  u32 t1;
  u32 t2;
  u32 t3;
  u32 t4;
  u32 t5;
  u32 t6;
  u32 a0;
  u32 a1;
  u32 a2;
  u32 a3;
  u32 a4;
  u32 a5;
  u32 a6;
  u32 a7;
  u32 s0;
  u32 s1;
  u32 s2;
  u32 s3;
  u32 s4;
  u32 s5;
  u32 s6;
  u32 s7;
  u32 s8;
  u32 s9;
  u32 s10;
  u32 s11;
  u32 sp;
} __attribute__((packed)) TrapFrame;

#define m_read_csr(reg)                                                        \
  ({                                                                           \
    u32 m__tmp;                                                                \
    __asm__ __volatile__("csrr %0, " #reg : "=r"(m__tmp));                     \
    m__tmp;                                                                    \
  })

#define m_write_csr(reg, value)                                                \
  do {                                                                         \
    u32 m__tmp = (value);                                                      \
    __asm__ __volatile__("csrw " #reg ", %0" ::"r"(m__tmp));                   \
  } while (0)

void handle_trap(TrapFrame *f) {
  (void)(f);
  u32 scause = m_read_csr(scause);
  u32 stval = m_read_csr(stval);
  u32 user_pc = m_read_csr(sepc);

  m_panic("unexpected trap scause=0x%x, stval=0x%x, sepc=0x%x\n", scause, stval,
          user_pc);
}

extern u8 __free_ram[], __free_ram_end[];

#define PAGE_SIZE 4096U

size_t alloc_pages(u32 n) {
  static size_t next_addr = (size_t) __free_ram;
  const size_t result = next_addr;
  next_addr += n * PAGE_SIZE;

  if(next_addr > (size_t)__free_ram_end) {
    m_panic("out of memory");
  }

  memset((void *) result, 0, n * PAGE_SIZE);
  return result;
}

void kernel_main(void) {
  const size_t bss_len = (size_t)__bss_end - (size_t)__bss_start;
  memset((void *)__bss_start, 0, bss_len);

  m_write_csr(stvec, ((size_t)(kernel_trap_trampoline)));

  const char *boot_msg = "\n\nHello, World!\n";
  for (size_t i = 0; boot_msg[i] != '\0'; i++) {
    putchar(boot_msg[i]);
  }

  printf("1 + 2 = %d, 0x%x\n", 1 + 2, 0x1234abcd);
  printf("Goodbye, %s\n\n", "World?");

  
  size_t addr0 = alloc_pages(2);
  size_t addr1 = alloc_pages(1);
  printf("alloc_pages test: addr0=0x%x\n",addr0);
  printf("alloc_pages test: addr1=0x%x\n",addr1);

  m_panic("!BOOTED!");
}
