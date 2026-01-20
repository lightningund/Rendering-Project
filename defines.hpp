#pragma once

#define PI 3.14159265358

#define WAIT_FOR_VAL(src, msk) while (((src) & (msk)) != msk) {}
#define WAIT_FOR_NOT_VAL(src, msk) while (((src) & (msk)) == msk) {}

using idx_t = uint32_t;
using TexID = uint8_t;