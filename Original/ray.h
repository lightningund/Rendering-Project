#pragma once

#include "vec.h"
#include "cell.h"

struct _Ray {
	Vec pos;
	Vec dir;
	Cell* cell_hit;
	uint8_t cell_wall;
	float dist;
	float t;
} typedef Ray;

void ray_cast_cell(Ray* ray, Cell* cell);