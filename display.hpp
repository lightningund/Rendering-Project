#pragma once

#include "defines.hpp"
#include <cstdint>
#include <SFML/Graphics/Color.hpp>

struct ScreenRect {
	uint8_t ax;
	uint8_t ay;
	uint8_t bx;
	uint8_t by;
};

using color_t = sf::Color;

#define RGB_TO_DEV(r, g, b) (sf::Color((r), (g), (b)))

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
// Clear the screen (does not affect the framebuffer)
void display_clear();
// Clear a section of the screen (does not affect the framebuffer)
void display_clear_section(ScreenRect dim);
// Update the entire screen to reflect the framebuffer
void display_update_screen();
// Update only a section of the screen
void display_update_section(ScreenRect dim);
// Update a single pixel in the framebuffer
void display_set_pixel(uint8_t x, uint8_t y, color_t color);
