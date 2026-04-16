#include "display.hpp"
#include <SFML/Graphics/Image.hpp>

sf::Image buffer;

// Initialize the display, must be called first
void display_init() {
	buffer = sf::Image{sf::Vector2u{WIDTH, HEIGHT}};
}

void display_set_pixel(size_t x, size_t y, color_t color) {
	buffer.setPixel(sf::Vector2u{x, y}, color);
}
