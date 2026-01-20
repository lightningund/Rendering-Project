#pragma once
#include "vec.h"
#include <stdint.h>

void init_stick();
Vec sample_stick();
void sample_stick_two(uint16_t* x, uint16_t* y);
