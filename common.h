#ifndef _COMMON_H_
#define _COMMON_H_
#include<stdint.h>
typedef uint32_t size_t;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint8_t u8;
typedef int8_t s8;

#define true 1
#define false 0
#define NULL ((void*)0)
#define m_align_up(value, align) __builtin_align_up(value,align)
#define m_is_aligned(value, align) __builtin_is_aligned(value,align)
#define m_offset_of(type, field) __builtin_offsetof(type,field)

#define va_list __builtin_va_list
#define m_va_start __builtin_va_start
#define m_va_end __builtin_va_end
#define m_va_arg __builtin_va_arg



void putchar(char ch);
void printf(const char *fmt, ...);
void memset(void *buf, uint8_t val, size_t n); 
void memcpy(void *dst, const void *src, size_t n); 
void strcpy(char *dst, const char *src, size_t n); 
int strcmp(const char *s1, const char *s2, size_t n); 

#endif
