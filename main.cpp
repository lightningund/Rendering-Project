#include "vec.hpp"
#include "ray.hpp"
#include "cell.hpp"
#include "display.hpp"
// #include "thumbstick.hpp"
#include "map.hpp"
#include "defines.hpp"
#include "textures.hpp"
#include "task.hpp"
#include <iostream>
#include <SFML/Graphics.hpp>

extern sf::Image buffer;

void event_loop(sf::Window& window) {
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
		}
	}
}

int main() {
	sf::RenderWindow window{sf::VideoMode{800, 800}, "[TITLE]"};
	window.setFramerateLimit(144);

	display_init();

	sf::Texture tex;
	tex.create(DISP_BUF_SIZE, DISP_BUF_SIZE);
	tex.setSmooth(false);

	display_set_pixel(40, 40, RGB_TO_DEV(255, 255, 255));

	sf::Sprite spr{tex};
	spr.setScale(5, 5);

	while (window.isOpen()) {
		event_loop(window);

		window.clear();

		tex.update(buffer);
		window.draw(spr);

		window.display();
	}

	return 0;
}