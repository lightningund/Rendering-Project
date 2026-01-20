#pragma once

#include "vec.hpp"
#include "cell.hpp"

struct Ray {
	Vec pos;
	Vec dir;
	Cell* cell_hit;
	uint8_t cell_wall;
	float dist;
	float t;
};

void ray_cast_cell(Ray* ray, Cell* cell);