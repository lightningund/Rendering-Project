#pragma once

#include "defines.hpp"
#include <SFML/Graphics/Color.hpp>

using color_t = sf::Color;

#define RGB_TO_DEV(r, g, b) (color_t{(r), (g), (b)})

static inline color_t color_mult(color_t src, float scalar) {
	uint8_t red = src.r * scalar;
	uint8_t green = src.g * scalar;
	uint8_t blue = src.b * scalar;
	return RGB_TO_DEV(red, green, blue);
}

static inline color_t fast_color_scale(color_t src, int div) {
	uint8_t red = src.r / div;
	uint8_t green = src.g / div;
	uint8_t blue = src.b / div;
	return RGB_TO_DEV(red, green, blue);
}

static const color_t BLACK = RGB_TO_DEV(0, 0, 0);
static const color_t GRAY = RGB_TO_DEV(128, 128, 128);
static const color_t GREY = RGB_TO_DEV(128, 128, 128);
static const color_t WHITE = RGB_TO_DEV(255, 255, 255);
static const color_t RED = RGB_TO_DEV(255, 0, 0);
static const color_t GREEN = RGB_TO_DEV(0, 255, 0);
static const color_t BLUE = RGB_TO_DEV(0, 0, 255);
static const color_t PINK = RGB_TO_DEV(136, 24, 96);
static const color_t ORANGE = RGB_TO_DEV(248, 228, 16);

// Initialize the display, must be called first
void display_init();
// Update a single pixel in the framebuffer
void display_set_pixel(size_t x, size_t y, color_t color);
