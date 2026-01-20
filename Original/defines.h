#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

#define PI 3.14159265358

#define WAIT_FOR_VAL(src, msk) while (((src) & (msk)) != msk) {}
#define WAIT_FOR_NOT_VAL(src, msk) while (((src) & (msk)) == msk) {}

typedef uint32_t idx_t;
typedef uint8_t TexID;

#ifndef NDEBUG
#include "../dcc_stdio.h"
static inline void write_u32(const uint32_t* val, unsigned long len) {
	dbg_write_u32(val, len);
}
static inline void write_u16(const uint16_t* val, unsigned long len) {
	dbg_write_u16(val, len);
}
static inline void write_u8(const uint8_t* val, unsigned long len) {
	dbg_write_u8(val, len);
}
static inline void write_str(const char* msg) {
	dbg_write_str(msg);
}
static inline void write_char(char msg) {
	dbg_write_char(msg);
}
#else
static inline void write_u32(const uint32_t* _v, unsigned long _l) {}
static inline void write_u16(const uint16_t* _v, unsigned long _l) {}
static inline void write_u8(const uint8_t* _v, unsigned long _l) {}
static inline void write_str(const char* _m) {}
static inline void write_char(char _c) {}
#endif