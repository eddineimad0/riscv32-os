#include "common.h"

void memset(void *buf, uint8_t val, size_t n) {
  uint8_t *p = (uint8_t *)buf;
  for (size_t i = 0; i < n; i++) {
    p[i] = val;
  }
}

void memcpy(void *dst, const void *src, size_t n) {
  u8 *d = (u8*) dst;
  const u8 *s = (const u8*) src;
  while(n--){
    *d++ = *s++;
  }
} 

void strcpy(char *dst, const char *src, size_t n) {
  while(n--){
    *dst++ = *src++;
  }
  *dst = '\0';
} 

s32 strcmp(const char *s1, const char *s2, size_t n) {
  s32 result = 0; 
  while(n--){
    if(*s1 != *s2){
      result = *s1 - *s2;
      break;
    }

    s1++;
    s2++;
  }
  return result;
} 
