#include "ray.hpp"
#include "defines.hpp"

void ray_cast_cell(Ray* ray, Cell* cell) {
	Vec offset = Vec{cell->x, cell->y};

	Vec scaled_pos = ray->pos / CELL_SIZE;

	for (int i = 0; i < 4; ++i) { // Loop through all the walls
		bool is_fake = cell_wall_is_fake(*cell->walls, i);
		// Only continue if the wall is either there or fake
		if (!(is_fake || cell_wall_is_there(*cell->walls, i))) continue;
		// If the wall is fake but we already hit something, show the thing behind it
		if (is_fake && ray->cell_hit != NULL) continue;

		Vec wall_delta = CellWallOffsets[i][1] - CellWallOffsets[i][0];

		float den = wall_delta.cross(ray->dir);
		if (den == 0) continue; // We have to divide by this so if it's zero then just give up
		// Of course, the real reason is that if this is zero then the lines are parallel

		Vec v1 = offset + CellWallOffsets[i][0];
		Vec origin_diff = v1 - scaled_pos;

		float u = wall_delta.cross(origin_diff) * CELL_SIZE / den;
		if (u <= 0 || u >= ray->dist) continue; // Ignore this if the intersection is actually behind us or it's further than what we've already hit

		float t = ray->dir.cross(origin_diff) / den;
		if (t <= 0 || t >= 1) continue; // Also ignore it if the intersection would be outside the bounds of the line

		// If we've gotten here, then we've got a new closest intersection
		ray->dist = u;
		ray->cell_hit = cell;
		ray->cell_wall = i;
		ray->t = t;
	}
}
