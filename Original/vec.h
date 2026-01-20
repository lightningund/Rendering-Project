#pragma once

struct _Vec {
	float x;
	float y;
} typedef Vec;

Vec vec_from_heading(const float angle);
Vec vec_negate(const Vec v);
Vec vec_normalize(const Vec v);
Vec vec_scale(const Vec v, const float s);
Vec vec_add(const Vec a, const Vec b);
Vec vec_sub(const Vec a, const Vec b);
float vec_sq_len(const Vec v);
float vec_len(const Vec v);
float vec_dot(const Vec a, const Vec b);
float vec_sq_dist(const Vec a, const Vec b);
float vec_dist(const Vec a, const Vec b);
float vec_cross(const Vec a, const Vec b);