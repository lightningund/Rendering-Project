#include "vec.hpp"
#include <cmath>

Vec Vec::from_heading(const float angle) {
	return { sin(angle), cos(angle) };
}

float Vec::sq_len() const {
	return (x * x) + (y * y);
}
float Vec::len() const {
	return hypot(x, y);
}
float Vec::sq_dist(const Vec& b) const {
	return ((*this) - b).sq_len();
}
float Vec::dist(const Vec& b) const {
	return ((*this) - b).len();
}
float Vec::cross(const Vec& b) const {
	return (x * b.y) - (y * b.x);
}

Vec Vec::normalize() const {
	auto scale = len();
	return (*this) / scale;
}

Vec vec_from_heading(const float angle) {
	return Vec{
		sin(angle),
		cos(angle)
	};
}

Vec vec_negate(const Vec v) {
	return Vec{-v.x, -v.y};
}

Vec vec_normalize(const Vec v) {
	return vec_scale(v, 1 / vec_len(v));
}

Vec vec_scale(const Vec v, const float s) {
	return Vec{
		v.x * s,
		v.y * s
	};
}

Vec vec_add(const Vec a, const Vec b) {
	return Vec{
		a.x + b.x,
		a.y + b.y
	};
}

Vec vec_sub(const Vec a, const Vec b) {
	return Vec{
		a.x - b.x,
		a.y - b.y
	};
}

float vec_sq_len(const Vec v) {
	return (v.x * v.x) + (v.y * v.y);
}

float vec_len(const Vec v) {
	return sqrtf(vec_sq_len(v));
}

float vec_dot(const Vec a, const Vec b) {
	return (a.x * b.x) + (a.y * b.y);
}

float vec_sq_dist(const Vec a, const Vec b) {
	Vec diff = vec_sub(a, b);
	return vec_sq_len(diff);
}

float vec_dist(const Vec a, const Vec b) {
	return sqrtf(vec_sq_dist(a, b));
}

float vec_cross(const Vec a, const Vec b) {
	return (a.x * b.y) - (a.y * b.x);
}
