#include "map.h"
#include "cell.h"
#include "display.h"
#include "textures.h"
#include <string.h>

static Vec prev_map_pos;

static color_t textures[] = {RED, GREEN, BLUE, WHITE};

// Takes in an x and y (at the correct map scale) and draws them to the screen
// There are special transforms that are done here so it's convenient to have it in one place
static void draw_on_map(int x, int y, uint16_t color) {
	display_set_pixel(y, x + (DISPLAY_SIZE / 2), color);
}

// Takes in an x and y in world space and converts them into x and y in map space
static Vec world_to_map(Vec world_pos) {
	return vec_scale(world_pos, ((float)DISPLAY_SIZE - 1) / CELL_GRID_DIM_Y / CELL_SIZE);
}

static void draw_cell(const Cell c) {
	float grid_size = (DISPLAY_SIZE) / CELL_GRID_DIM_Y;

	int ax = c.x * grid_size;
	int ay = c.y * grid_size;

	for (int i = 0; i < 4; ++i) { // Loop through all the walls
		int wa[2];
		int delta[2];
		memcpy(wa, MapOffsets[i][0], sizeof(int) * 2);
		memcpy(delta, MapOffsets[i][1], sizeof(int) * 2);

		wa[0] *= grid_size;
		wa[1] *= grid_size;

		uint16_t wall_col = textures[cell_wall_texture(*c.walls, i)];

		// If it's not there then we'll just draw it black
		if (!cell_wall_is_there(*c.walls, i) || cell_wall_is_fake(*c.walls, i)) {
			wall_col = 0;
		}

		for (int i = 0; i < grid_size; ++i) {
			draw_on_map(wa[0] + ax + delta[0] * i, wa[1] + ay + delta[1] * i, wall_col);
		}
	}
}

void map_draw(Vec cam) {
	for (int i = 0; i < MAX_CELLS; ++i) {
		if (cell_free(i)) continue;
		draw_cell(cells[i]);
	}

	// Clear the previous player position off the map
	draw_on_map(prev_map_pos.x, prev_map_pos.y, 0);

	// Render the player on the map
	Vec map_pos = world_to_map(cam);
	prev_map_pos = map_pos;
	draw_on_map(map_pos.x, map_pos.y, PINK);

	display_update_section((ScreenRect) {
		.ax = 16,
		.ay = 96 / 2,
		.bx = 112,
		.by = 96
	});
}