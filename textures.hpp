#pragma once
#include "display.hpp"

// Goes through these in order while turning clockwise (if looking from above)
// static color_t textures[] = {RED, GREEN, BLUE, WHITE};

color_t get_tex(uint8_t tex_id, int extent, int y, float t);
