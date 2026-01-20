#include "cell.h"
#include <stdlib.h>
#include "defines.h"

const Vec CellWallOffsets[4][2] = {
	{{0, 1}, {1, 1}},
	{{1, 1}, {1, 0}},
	{{1, 0}, {0, 0}},
	{{0, 0}, {0, 1}}
};

const unsigned int CellWallBits = 4;

const int16_t DirOffsets[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

const int MapOffsets[4][2][2] = {
	{{0, 1}, {1, 0}},
	{{1, 1}, {0, -1}},
	{{1, 0}, {-1, 0}},
	{{0, 0}, {0, 1}}
};

// Bitset to represent which indices have real cells
idx_t used_cells = 0;
// Constant representing what `used_cells` will look like when full
const idx_t full_cell_arr = (1UL << MAX_CELLS) - 1;

GridCell cell_grid[CELL_GRID_DIM_X][CELL_GRID_DIM_Y];

Cell cells[MAX_CELLS];

// The direction that the ray came from for the new cell
int TBG_DIR;
// The x and y coords of the new cell
int TBG_X;
int TBG_Y;

void init_grid() {
	for (int i = 0; i < CELL_GRID_DIM_X; ++i) {
		for (int j = 0; j < CELL_GRID_DIM_Y; ++j) {
			for (int w = 0; w < 4; ++w) {
				uint8_t tex = rand() % 4;
				make_cell_wall_real(&cell_grid[i][j], w);
				make_cell_wall_there(&cell_grid[i][j], w);
				set_cell_wall_texture(&cell_grid[i][j], w, tex);
			}
		}
	}
}

bool cell_free(idx_t idx) {
	return (used_cells & (1 << idx)) == 0;
}

bool cells_full() {
	return used_cells == full_cell_arr;
}

idx_t find_open_cell() {
	if (cells_full()) return -1;

	for (int i = 0; i < MAX_CELLS; ++i) {
		if (cell_free(i)) return i;
	}

	return -1;
}

void remove_cell(idx_t idx) {
	// If the cell isn't actually there, we can't do anything
	if (cell_free(idx)) return;

	// Remove it from the bitset
	used_cells &= (idx_t)(-1) ^ (1 << idx);

	// If removing this cell would give us no cells left, then stop
	if (used_cells == 0) {
		used_cells |= (1 << idx);
		return;
	}

	write_str("Removing a cell\n");

	Cell rem = cells[idx];

	if (rem.y > 0) {
		GridCell* neighbor = &(cell_grid[rem.x][rem.y - 1]);
		if (!cell_wall_is_there(*neighbor, 0)) {
			make_cell_wall_fake(neighbor, 0);
		}
	}

	if (rem.x > 0) {
		GridCell* neighbor = &(cell_grid[rem.x - 1][rem.y]);
		if (!cell_wall_is_there(*neighbor, 1)) {
			make_cell_wall_fake(neighbor, 1);
		}
	}

	if (rem.y < CELL_GRID_DIM_Y - 1) {
		GridCell* neighbor = &(cell_grid[rem.x][rem.y + 1]);
		if (!cell_wall_is_there(*neighbor, 2)) {
			make_cell_wall_fake(neighbor, 2);
		}
	}

	if (rem.x < CELL_GRID_DIM_X - 1) {
		GridCell* neighbor = &(cell_grid[rem.x + 1][rem.y]);
		if (!cell_wall_is_there(*neighbor, 3)) {
			make_cell_wall_fake(neighbor, 3);
		}
	}

	for (int w = 0; w < 4; ++w) {
		make_cell_wall_there(rem.walls, w);
	}
}

// Instead of generating random walls, make it function sort of like a snake where only one wall is open
// Make sure that we don't choose a direction that already has a block behind it
// Also make sure that both ends are open
// What happens if we hit a dead end?
// 		Well then all rays will be inside this block and so we can just turn around and generate it again
void generate_cell() {
	idx_t idx = find_open_cell();

	if (idx == -1) return;

	for (int i = 0; i < MAX_CELLS; ++i) {
		if (cell_free(i)) continue;

		// If we already have another cell here, don't bother
		if (cells[i].x == TBG_X && cells[i].y == TBG_Y) return;
	}

	GridCell* TBG = &cell_grid[TBG_X][TBG_Y];
	// Set it to all 1s except for the one connecting it to the previous cell
	*TBG &= (uint16_t)(-1) ^ (0xF << (CellWallBits * ((TBG_DIR + 2) % 4)));

	uint8_t avail_walls = 0;

	if (TBG_Y < CELL_GRID_DIM_Y - 1) {
		avail_walls |= (1 << 0);
	}

	if (TBG_X < CELL_GRID_DIM_X - 1) {
		avail_walls |= (1 << 1);
	}

	if (TBG_Y > 0) {
		avail_walls |= (1 << 2);
	}

	if (TBG_X > 0) {
		avail_walls |= (1 << 3);
	}

	for (int i = 0; i < 4; ++i) {
		if (!cell_wall_is_fake(*TBG, i) && !cell_wall_is_there(*TBG, i)) {
			avail_walls &= (uint8_t)(-1) ^ (1 << i);
		}
	}

	// write_str("Walls We Can Use: ");
	// write_u8(&(avail_walls), 1);
	// write_char('\n');

	// Pick a wall to remove
	if (avail_walls != 0) {
		uint8_t rng;

		// Very convoluted way of determining a random wall from the available ones
		int num_options = 0;
		for (int i = 0; i < 4; ++i) {
			if (avail_walls & (1 << i)) {
				++num_options;
			}
		}

		rng = rand() % num_options;

		for (int i = 0; i < 4; ++i) {
			if (avail_walls & (1 << i)) {
				if (rng == 0) {
					rng = i;
					break;
				}
				--rng;
			}
		}

		make_cell_wall_fake(TBG, rng);
		make_cell_wall_gone(TBG, rng);
		GridCell* neighbor = &cell_grid[TBG_X + DirOffsets[rng][0]][TBG_Y + DirOffsets[rng][1]];
		make_cell_wall_fake(neighbor, (rng + 2) % 4);
		make_cell_wall_gone(neighbor, (rng + 2) % 4);
	}

	// Make sure all the other walls are set to real
	for (int i = 0; i < 4; ++i) {
		// If it's both there and fake, make it just real
		if (cell_wall_is_fake(*TBG, i) && cell_wall_is_there(*TBG, i)) {
			make_cell_wall_real(TBG, i);
		}
	}

	// write_str("Generated Walls: ");
	// write_u16(TBG, 1);
	// write_str("\nX Coord: ");
	// write_u16(&TBG_X, 1);
	// write_str("\nY Coord: ");
	// write_u16(&TBG_Y, 1);
	// write_char('\n');

	cells[idx] = (Cell) {
		.x = TBG_X,
		.y = TBG_Y,
		.walls = TBG,
		.hit = false
	};

	used_cells |= (1 << idx);
}
