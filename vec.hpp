#pragma once

struct Vec {
	float x;
	float y;

	constexpr Vec() = default;

	template <typename T>
	constexpr Vec(const T& x, const T& y) : x{static_cast<float>(x)}, y{static_cast<float>(y)} {}

	template <typename T>
	constexpr Vec(const T& x) : x{static_cast<float>(x)}, y{static_cast<float>(x)} {}

	static Vec from_heading(const float angle);

	constexpr Vec operator-() const {
		return { -x, -y };
	}

	constexpr Vec& operator+=(const Vec& b) {
		x += b.x;
		y += b.y;
		return *this;
	}
	constexpr Vec& operator-=(const Vec& b) {
		x -= b.x;
		y -= b.y;
		return *this;
	}
	constexpr Vec& operator*=(const float s) {
		x *= s;
		y *= s;
		return *this;
	}
	constexpr Vec& operator/=(const float s) {
		x /= s;
		y /= s;
		return *this;
	}

	friend constexpr Vec operator+(Vec a, const Vec& b) {
		a += b;
		return a;
	}
	friend constexpr Vec operator-(Vec a, const Vec& b) {
		a -= b;
		return a;
	}
	friend constexpr Vec operator*(Vec a, const float b) {
		a *= b;
		return a;
	}
	friend constexpr Vec operator/(Vec a, const float b) {
		a /= b;
		return a;
	}

	constexpr float operator*(const Vec& b) const {
		return (x * b.x) + (y * b.y);
	}

	float sq_len() const;
	float len() const;
	float sq_dist(const Vec& b) const;
	float dist(const Vec& b) const;
	float cross(const Vec& b) const;

	Vec normalize() const;
};