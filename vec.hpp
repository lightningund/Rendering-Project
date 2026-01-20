#pragma once

struct Vec {
	float x;
	float y;

	static constexpr Vec from_heading(const float angle);
	static constexpr Vec negate(const Vec v);
	static constexpr Vec normalize(const Vec v);
	static constexpr Vec scale(const Vec v, const float s);
	static constexpr Vec add(const Vec a, const Vec b);
	static constexpr Vec sub(const Vec a, const Vec b);
	static constexpr float sq_len(const Vec v);
	static constexpr float len(const Vec v);
	static constexpr float dot(const Vec a, const Vec b);
	static constexpr float sq_dist(const Vec a, const Vec b);
	static constexpr float dist(const Vec a, const Vec b);
	static constexpr float cross(const Vec a, const Vec b);
};

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