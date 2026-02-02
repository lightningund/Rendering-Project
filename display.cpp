#include "display.hpp"
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

sf::Image buffer;

// Initialize the display, must be called first
void display_init() {
	buffer.create(WIDTH, HEIGHT);
}

void display_set_pixel(uint8_t x, uint8_t y, color_t color) {
	buffer.setPixel(x, y, color);
}
