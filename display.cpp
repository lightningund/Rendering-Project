#include "display.hpp"
#include <SFML/Graphics/Image.hpp>

sf::Image buffer;

// Initialize the display, must be called first
void display_init() {
	buffer.create(WIDTH, HEIGHT);
}

void display_set_pixel(size_t x, size_t y, color_t color) {
	buffer.setPixel(x, y, color);
}
