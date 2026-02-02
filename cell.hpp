#pragma once

#include "vec.hpp"
#include "defines.hpp"

#define CELL_SIZE 20

// The 4 walls are 4 bits each
// North, East, South, West
// With North being (0,+1) and East being (+1,0)
// The lower two bits of the walls determine whether they are real, and whether they are there at all
// A wall not being real means that there is simply an ungenerated cell on the other side
// The upper two bits are the texture ID
typedef uint16_t GridCell;

struct Cell {
	GridCell* walls;
	uint16_t x;
	uint16_t y;
	bool hit;
};

constexpr auto MAX_CELLS = 30;

extern const Vec CellWallOffsets[4][2];
extern const unsigned int CellWallBits;
extern const int16_t DirOffsets[4][2];
// Walls -> [Start, Delta] -> [x, y]
extern const int MapOffsets[4][2][2];

// Bitset to represent which indices have real cells
extern idx_t used_cells;
// Constant representing what `used_cells` will look like when full
extern const idx_t full_cell_arr;

constexpr auto CELL_GRID_DIM_X = 8;
constexpr auto CELL_GRID_DIM_Y = (CELL_GRID_DIM_X * 2);
// The real cells that make up the grid
extern GridCell cell_grid[CELL_GRID_DIM_X][CELL_GRID_DIM_Y];

// Pointers to the cells that are currently active (and should be rendered)
extern Cell cells[MAX_CELLS];

// The direction that the ray came from for the new cell
extern int TBG_DIR;
// The x and y coords of the new cell
extern uint16_t TBG_X;
extern uint16_t TBG_Y;

#define WALL_THERE_BIT 0b0001
#define WALL_FAKE_BIT 0b0010

// Does the bit tomfoolery to get just the bits representing the wall at an index
constexpr GridCell cell_wall_section(const GridCell wall, const idx_t dir) {
	return (wall >> (CellWallBits * dir)) & 0xF;
}

// Gets the texture bits of the wall section
constexpr uint8_t cell_wall_texture(const GridCell wall, const idx_t dir) {
	return cell_wall_section(wall, dir) >> 2;
}

// Tests the fake bit of the wall section
constexpr bool cell_wall_is_fake(const GridCell wall, const idx_t dir) {
	return (cell_wall_section(wall, dir) & WALL_FAKE_BIT) != 0;
}

// Tests the "there" bit of the wall section
constexpr bool cell_wall_is_there(const GridCell wall, const idx_t dir) {
	return (cell_wall_section(wall, dir) & WALL_THERE_BIT) != 0;
}

// Sets the fake bit of the wall section
constexpr void make_cell_wall_fake(GridCell* wall, const idx_t dir) {
	*wall |= WALL_FAKE_BIT << (CellWallBits * dir);
}

// Unsets the fake bit of the wall section
constexpr void make_cell_wall_real(GridCell* wall, const idx_t dir) {
	*wall &= (GridCell)(-1) ^ (WALL_FAKE_BIT << (CellWallBits * dir));
}

// Sets the "there" bit of the wall section
constexpr void make_cell_wall_there(GridCell* wall, const idx_t dir) {
	*wall |= WALL_THERE_BIT << (CellWallBits * dir);
}

// Unsets the "there" bit of the wall section
constexpr void make_cell_wall_gone(GridCell* wall, const idx_t dir) {
	*wall &= (GridCell)(-1) ^ (WALL_THERE_BIT << (CellWallBits * dir));
}

constexpr void set_cell_wall_texture(GridCell* wall, const idx_t dir, TexID tex) {
	// Clear the texture first
	*wall &= (GridCell)(-1) ^ (0b1100 << (CellWallBits * dir));
	// Set the actual texture
	*wall |= tex << (2 + CellWallBits * dir);
}

constexpr bool inside_cell(const Cell c, const Vec p) {
	return
		((c.x + 1) * CELL_SIZE) > p.x &&
		(c.x * CELL_SIZE) < p.x &&
		((c.y + 1) * CELL_SIZE) > p.y &&
		(c.y * CELL_SIZE) < p.y;
}

// Make sure all the cells start off with all their walls
void init_grid();

// Check if a given cell is free
bool cell_free(idx_t idx);
// Check if the cell array is at max
bool cells_full();
// Find the next open cell index
idx_t find_open_cell();
// Remove a cell, and mark the empty walls around it as fake
void remove_cell(idx_t idx);
// Finish generating the `to_be_generated` cell
void generate_cell();