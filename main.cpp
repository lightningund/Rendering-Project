#include <iostream>
#include <SFML/Graphics.hpp>

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

	while (window.isOpen()) {
		event_loop(window);

		window.clear();

		window.display();
	}

	return 0;
}