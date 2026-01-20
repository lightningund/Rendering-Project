#pragma once

struct Vec {
	float x;
	float y;

	static Vec from_heading(const float angle);

	constexpr Vec operator-() const;
	constexpr Vec operator+(const Vec& other) const;
	constexpr Vec operator-(const Vec& other) const;
	constexpr Vec operator*(const float s) const;
	constexpr Vec operator/(const float s) const;
	constexpr float operator*(const Vec& other) const;

	constexpr float sq_len() const;
	constexpr float len() const;
	constexpr float sq_dist(const Vec& b) const;
	constexpr float dist(const Vec& b) const;
	constexpr float cross(const Vec& b) const;

	constexpr Vec normalize();
};

[[deprecated]] Vec vec_from_heading(const float angle);
[[deprecated]] Vec vec_negate(const Vec v);
[[deprecated]] Vec vec_normalize(const Vec v);
[[deprecated]] Vec vec_scale(const Vec v, const float s);
[[deprecated]] Vec vec_add(const Vec a, const Vec b);
[[deprecated]] Vec vec_sub(const Vec a, const Vec b);
[[deprecated]] float vec_sq_len(const Vec v);
[[deprecated]] float vec_len(const Vec v);
[[deprecated]] float vec_dot(const Vec a, const Vec b);
[[deprecated]] float vec_sq_dist(const Vec a, const Vec b);
[[deprecated]] float vec_dist(const Vec a, const Vec b);
[[deprecated]] float vec_cross(const Vec a, const Vec b);