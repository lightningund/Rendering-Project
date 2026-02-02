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