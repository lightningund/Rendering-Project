#pragma once

#include <cstdint>

struct ScreenRect {
	uint8_t ax;
	uint8_t ay;
	uint8_t bx;
	uint8_t by;
};

struct Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

#define DISPLAY_SIZE 96
#define DISP_BUF_SIZE 128

typedef uint16_t color_t;

#define RGB_TO_DEV(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

static inline color_t color_mult(color_t src, float scalar) {
	color_t red = ((src >> 11) & 0x1F) * scalar;
	color_t green = ((src >> 5) & 0x3F) * scalar;
	color_t blue = (src & 0x1F) * scalar;
	return red << 11 | green << 5 | blue;
}

static inline color_t fast_color_scale(color_t src, int div) {
	color_t red = ((src >> 11) & 0x1F) / div;
	color_t green = ((src >> 5) & 0x3F) / div;
	color_t blue = (src & 0x1F) / div;
	return red << 11 | green << 5 | blue;
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

extern color_t pixels[DISP_BUF_SIZE * DISP_BUF_SIZE];

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
