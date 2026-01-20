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
#include <optional>
#include <SFML/Graphics.hpp>

extern sf::Image buffer;

#define WIDTH 96
#define HEIGHT 96
// Field of View (in degrees)
#define FOV 110.0

// How much to turn whenever you press a button (in radians)
#define TURN_AMNT (PI / 8)

struct Column {
	int extent;
	int prev_extent;
	color_t col;
	uint8_t tex_id;
	float t;
	int shade;
};

#define NUM_TEX 4

bool wall_textured = true;

static color_t textures[] = {RED, GREEN, BLUE, WHITE};

float ang_height;
float ang_height_sq;
float ang_height_sq_inv;

Vec cam_pos{10, 5};
float cam_ang = 0;

Ray rays[WIDTH];
Column cols[WIDTH];

bool to_be_generated;

float radians(float deg) {
	return (deg / 180.0) * PI;
}

void read_btns() {
	// TODO
	// Board Button
	// if (btn_rising & (1 << 15)) {
	// 	wall_textured = !wall_textured;
	// }
}

void draw_render() {
	for (int y = 0; y < HEIGHT / 2; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			if (cols[x].extent < y && cols[x].extent < (HEIGHT / 2) - y) {
				if (wall_textured) {
					color_t color = get_tex(cols[x].tex_id, cols[x].extent, y, cols[x].t);
					display_set_pixel(x, y, fast_color_scale(color, cols[x].shade));
				} else {
					display_set_pixel(x, y, cols[x].col);
				}
			} else if (cols[x].prev_extent < y && cols[x].prev_extent < (HEIGHT / 2) - y) {
				display_set_pixel(x, y, BLACK);
			}
		}
	}

	for (int i = 0; i < WIDTH; ++i) {
		cols[i].prev_extent = cols[i].extent;
	}

	display_update_section(ScreenRect{
		.ax = 16,
		.ay = 0,
		.bx = 112,
		.by = 96 / 2
	});
}

std::optional<uint8_t> get_ray_wall(const Ray* ray) {
	if (ray->cell_hit == nullptr) return {};
	return cell_wall_section(*ray->cell_hit->walls, ray->cell_wall);
}

// Calculate the height and shade of a single pixel column based on the ray
void update_column(const Ray ray, int x) {
	static const float H4 = HEIGHT / 4;
	static const float W2 = WIDTH / 2;

	auto wall = get_ray_wall(&ray);

	std::cout << "Got ray intersection\n";

	// If it's fake then just blank it out and leave
	if (!wall || cell_wall_is_fake(wall.value(), 0)) {
		cols[x].extent = WIDTH;
		return;
	}

	auto ray_wall = wall.value();

    // float a = atan2(x - (WIDTH / 2), ang_height);
    // float d = cos(a) * ray.dist;
    // float diff = (HEIGHT / 4) - (atan(1 / d) * 200);
	float diff = H4 - 200 * atan(sqrt(1 + ((x - W2) * (x - W2) * ang_height_sq_inv)) / ray.dist);

	cols[x].extent = (int)diff;
	float shade = 1 - (3 * ray.dist / WIDTH);
	// Make the shade more pronounced
	shade *= shade;
	// shade *= shade;
	// Make the walls always at least a little bit visible
	if (shade < 0.05) shade = 0.05;
	// Limit it just in case
	if (shade > 1) shade = 1;
	if (wall_textured) {
		cols[x].shade = 1 / shade;
		cols[x].tex_id = cell_wall_texture(ray_wall, 0) % NUM_TEX;
		cols[x].t = ray.t;
	} else {
		cols[x].col = textures[cell_wall_texture(ray_wall, 0) % NUM_TEX];
		cols[x].col = color_mult(cols[x].col, shade);
	}
}

void update_columns() {
	for (int i = 0; i < WIDTH; ++i) {
		update_column(rays[i], i);
	}
}

void update_ray(Ray* ray) {
	ray->dist = INFINITY;
	ray->cell_hit = NULL;
	for (int j = 0; j < MAX_CELLS; ++j) {
		if (cell_free(j)) continue;

		ray_cast_cell(ray, &cells[j]);
	}

	if (ray->cell_hit != NULL) {
		ray->cell_hit->hit = true;

		// If we have room to make a new cell
		if (!cells_full()) {
			// If we don't already have another cell to generate
			if (!to_be_generated) {
				GridCell* walls = ray->cell_hit->walls;
				uint8_t dir = ray->cell_wall;
				// If the wall that the ray hit was fake
				if (cell_wall_is_fake(*walls, dir)) {
					// Set the wall on the cell we hit to be actually empty now instead of fake
					make_cell_wall_real(walls, dir);
					make_cell_wall_gone(walls, dir);
					TBG_DIR = dir;
					TBG_X = ray->cell_hit->x + DirOffsets[TBG_DIR][0];
					TBG_Y = ray->cell_hit->y + DirOffsets[TBG_DIR][1];
					to_be_generated = true;
				}
			}
		}
	}
}

void update_rays() {
	for (int i = 0; i < WIDTH; ++i) {
		rays[i].pos = cam_pos;
		rays[i].dir = vec_from_heading(cam_ang + atan2(i - WIDTH / 2, ang_height));
		update_ray(&rays[i]);
	}
}

void draw_map() {
	map_draw(cam_pos);
}

void update_cells() {
	for (int i = 0; i < MAX_CELLS; ++i) {
		if (cell_free(i)) continue;

		// Remove this cell if it wasn't hit by any rays and it's not the cell the player is currently in
		if (!cells[i].hit && !inside_cell(cells[i], cam_pos)) {
			remove_cell(i);
		}

		cells[i].hit = false;
	}
}

void move_cam() {
	// TODO
	// Vec stick = sample_stick();
	Vec stick{};
	// Center the range
	stick.x -= 0x800;
	stick.y -= 0x800;
	// Stick deadzone
	if (fabs(stick.x) < 0x100) stick.x = 0;
	if (fabs(stick.y) < 0x100) stick.y = 0;
	// Scale the range
	stick = vec_scale(stick, 1.0 / 0x800);
	// Turn the camera
	cam_ang += stick.x / 16;
	// Move the camera
	cam_pos = vec_add(cam_pos, vec_scale(vec_from_heading(cam_ang), -stick.y));
}

void gen_cell() {
	if (!to_be_generated) return;
	generate_cell();
	to_be_generated = false;
}

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

	init_grid();

	TBG_X = 0;
	TBG_Y = 0;
	TBG_DIR = 0;
	generate_cell();
	*cells[0].walls = 0xD952; // Only north wall fake

	ang_height = ((WIDTH / 2) * tan(radians(90 - (FOV / 2))));
	ang_height_sq = ang_height * ang_height;
	ang_height_sq_inv = 1.0 / ang_height_sq;

	std::cout << "Going to main loop\n";

	while (window.isOpen()) {
		event_loop(window);

		update_rays();
		std::cout << "Updated Rays\n";
		update_columns();
		std::cout << "Updated Columns\n";
		draw_render();
		std::cout << "Rendered Main Area\n";
		update_cells();
		std::cout << "Updated Cells\n";
		draw_map();
		std::cout << "Rendered Map\n";
		move_cam();
		std::cout << "Moved Camera\n";
		gen_cell();
		std::cout << "Generated New Cells\n";

		window.clear();

		tex.update(buffer);
		window.draw(spr);

		window.display();
	}

	return 0;
}