#include "display.hpp"
#include <SFML/Graphics/Image.hpp>

color_t pixels[DISP_BUF_SIZE * DISP_BUF_SIZE];

sf::Image buffer;

// Initialize the display, must be called first
void display_init() {
	buffer.create(DISP_BUF_SIZE, DISP_BUF_SIZE);
}

// Clear the screen (does not affect the framebuffer)
void display_clear() {}
// Clear a section of the screen (does not affect the framebuffer)
void display_clear_section(ScreenRect dim) {}
// Update the entire screen to reflect the framebuffer
void display_update_screen() {}
// Update only a section of the screen
void display_update_section(ScreenRect dim) {}

void display_set_pixel(uint8_t x, uint8_t y, color_t color) {
	buffer.setPixel(x, y, sf::Color(static_cast<uint32_t>(color)));
	// pixels[y * DISP_BUF_SIZE + x + 0x10] = color;
}
