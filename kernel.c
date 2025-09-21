#include<stdint.h>
typedef uint32_t size_t;



extern uint8_t *__bss_start, *__bss_end, *__stack_top;

void memset(void *buf, uint8_t val, size_t n) {
  uint8_t *p = (uint8_t*)buf;
  for (size_t i = 0; i<n; i++){
    p[i] = val;
  }
}

void kernel_main(void){
  const size_t bss_len = (size_t)__bss_end - (size_t)__bss_start;
  memset((void*)__bss_start,0, bss_len);

  for(;;){
    __asm__ __volatile__("nop");
  }
}

__attribute__((section(".text.boot")))
__attribute__((naked))
void boot(void){
  __asm__ __volatile__(
    "mv sp, %[stack_top]\n"
    "j kernel_main\n"
    :
    : [stack_top] "r" (__stack_top)
  );
}
