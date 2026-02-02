#pragma once
#include <cstdint>

#define PI 3.14159265358

#define WIDTH 512
#define HEIGHT 512

#define WAIT_FOR_VAL(src, msk) while (((src) & (msk)) != msk) {}
#define WAIT_FOR_NOT_VAL(src, msk) while (((src) & (msk)) == msk) {}

using idx_t = uint32_t;
using TexID = uint8_t;