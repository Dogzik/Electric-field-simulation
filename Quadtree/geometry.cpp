#include "geometry.h"
#include <cassert>
#include <algorithm>
#include <map>


bool is_zero(float x) {
	return abs(x) < eps;
}

Point::Point(float _x, float _y, float _z): x(_x), y(_y), z(_z) { }

Point::Point(): Point(0, 0, 0) { }

float& Point::operator[](size_t t) {
	return data[t];
}

float Point::operator[](size_t t) const {
	return data[t];
}

Point Point::operator-=(Point b) {
	return *this = *this - b;
}

Point Point::operator/(float c) const {
	return Point(x / c, y / c, z / c);
}

Point Point::operator*(float c) const {
	return Point(x * c, y * c, z * c);
}

Point cross_product(Point a, Point b) {
	// (a2b3  -   a3b2,     a3b1   -   a1b3,     a1b2   -   a2b1)
	return Point(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float dot_product(Point a, Point b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Triangle::Triangle(Point a, Point b, Point c) {
	points[0] = a;
	points[1] = b;
	points[2] = c;
}

Triangle::Triangle(Triangle const& b) {
	points[0] = b.points[0];
	points[1] = b.points[1];
	points[2] = b.points[2];
}

Triangle::iterator Triangle::begin() {
	return points;
}

Triangle::iterator Triangle::end() {
	return points + 3;
}

Point Triangle::operator[](size_t x) {
	assert(x < 3);
	return points[x];
}

Interval get_interval(Triangle triangle, Point axis) {
	Interval result;

	result.min = dot_product(axis, triangle.points[0]);
	result.max = result.min;
	for (int i = 1; i < 3; ++i) {
		float value = dot_product(axis, triangle.points[i]);
		result.min = fminf(result.min, value);
		result.max = fmaxf(result.max, value);
	}

	return result;
}

bool overlap_on_axis(Triangle t1, Triangle t2, Point axis) {
	Interval a = get_interval(t1, axis);
	Interval b = get_interval(t2, axis);
	return ((b.min <= a.max) && (a.min <= b.max));
}

bool cross_triangle_triangle(Triangle t1, Triangle t2) {
	Point t1_f0 = t1.b - t1.a; // Edges t1
	Point t1_f1 = t1.c - t1.b;
	Point t1_f2 = t1.a - t1.c;

	Point t2_f0 = t2.b - t2.a; // Edges t2
	Point t2_f1 = t2.c - t2.b;
	Point t2_f2 = t2.a - t2.c;

	Point axisToTest[] = {
		// Triangle 1, Normal
		cross_product(t1_f0, t1_f1),
		// Triangle 2, Normal
		cross_product(t2_f0, t2_f1),

		// Cross Product of edges
		cross_product(t2_f0, t1_f0),
		cross_product(t2_f0, t1_f1),
		cross_product(t2_f0, t1_f2),

		cross_product(t2_f1, t1_f0),
		cross_product(t2_f1, t1_f1),
		cross_product(t2_f1, t1_f2),

		cross_product(t2_f2, t1_f0),
		cross_product(t2_f2, t1_f1),
		cross_product(t2_f2, t1_f2),
	};

	for (int i = 0; i < 11; ++i) {
		if (!overlap_on_axis(t1, t2, axisToTest[i])) {
			return false; // Seperating axis found
		}
	}
	return true; // Seperating axis not found
}

Box::Box(Point first, Point second) :
	first(first),
	second(second) { }

bool Box::contains(Point x) const {
	for (int i = 0; i < 3; i++) {
		if (!(first[i] <= x[i] && x[i] <= second[i])) {
			return false;
		}
	}
	return true;
}

std::vector<Point> Box::get_points() const {
	std::vector<Point> temp;
	for (int i = 0; i < 8; i++) {
		Point p;
		for (int j = 0; j < 3; j++) {
			if (i & (1 << j)) {
				p[j] = first[j];
			}
			else {
				p[j] = second[j];
			}
		}
		temp.push_back(p);
	}
	return temp;
}

float tetrahedron_volume(Point a, Point b, Point c, Point d) {
	b -= d , c -= d , a -= d;
	return dot_product(a, cross_product(b, c)) / 6;
}

float tetrahedron_volume(Point a, Triangle tr) {
	return tetrahedron_volume(a, tr[0], tr[1], tr[2]);
}

Object::Object(const std::vector<Point>& points, std::vector<std::vector<int> > connect) :
	points(points) {
	for (auto v : connect) {
		polygones.push_back(Triangle(
			points[v[0]],
			points[v[1]],
			points[v[2]]
		));
	}
	init();
}

Object::iterator Object::begin() {
	return polygones.begin();
}

Object::iterator Object::end() {
	return polygones.end();
}

void Object::init() {
	// calc volume and unique points
	real_volume = 0;
	for (Triangle v : polygones) {
		real_volume += tetrahedron_volume(Point(), v);
		for (Point u : v) {
			points.push_back(u);
		}
	}
	real_volume = abs(real_volume);
	sort(points.begin(), points.end());
	points.resize(unique(points.begin(), points.end()) - points.begin());
}

Object::Object(const std::vector<Triangle>& trianguals): polygones(trianguals) {
	init();
}

Point get_neighbor_point(Point p, Box trianguals, int q, int i) {
	Point a = p;
	if (i & (1 << q)) {
		a[q] = trianguals.second[q];
	}
	else {
		a[q] = trianguals.first[q];
	}
	return a;
}

int bit_count(int x) {
	int r = 0;
	while (x > 0) {
		r += x & 1;
		x >>= 1;
	}
	return r;
}

Object::Object(Box rectangle) {
	Point mid = (rectangle.first + rectangle.second) / 2;
	for (int i = 0; i < 8; i++) {
		Point p;
		for (int j = 0; j < 3; j++) {
			if (i & (1 << j)) {
				p[j] = rectangle.first[j];
			}
			else {
				p[j] = rectangle.second[j];
			}
		}
		for (int j = 0; j < 3; j++) {
			int q = (j + 2) % 3;
			if (i == 0
				|| i == 7
				|| ((i ^ (1 << j)) != 0
					&& (i ^ (1 << j)) != 7
					&& (i ^ (1 << q)) != 0
					&& (i ^ (1 << q)) != 7)) {
				Point a = get_neighbor_point(p, rectangle, j, i);
				Point b = get_neighbor_point(p, rectangle, q, i);
				auto temp = tetrahedron_volume(mid, Triangle(a, b, p));
				if (temp > 0) {
					polygones.push_back(Triangle(a, b, p));
				}
				else {
					polygones.push_back(Triangle(a, p, b));
				}
			}
		}
	}
	init();
}

bool Object::contains(Point p) {
	// calculate volume
	float temp = 0;
	for (Triangle v : polygones) {
		temp += abs(tetrahedron_volume(p, v));
	}
	return is_zero(temp - real_volume);
}

CrossType Object::cross(Box limit) {
	std::vector<Point> limit_points = limit.get_points();
	Object limit_obj = limit;
	bool in = true;
	for (Point v : limit_points) {
		if (!contains(v)) {
			in = false;
		}
	}
	if (in) {
		return LIMIT_IN_OBJ;
	}
	bool out = true;
	for (Point tpoint : points) {
		if (!limit_obj.contains(tpoint)) {
			out = false;
		}
	}
	if (out) {
		return OBJ_IN_LIMIT;
	}
	for (Triangle v : limit_obj) {
		for (Triangle u : polygones) {
			if (cross_triangle_triangle(v, u)) {
				return INTERSECTION;
			}
		}
	}
	return EMPTY_INTERSECTION;
}

Point operator+(Point a, Point b) {
	return Point(a.x + b.x, a.y + b.y, a.z + b.z);
}

Point operator-(Point a, Point b) {
	return Point(a.x - b.x, a.y - b.y, a.z - b.z);
}

bool operator<(Point a, Point b) {
	for (int i = 0; i < 3; i++) {
		if (!is_zero(a[i] - b[i])) {
			return a[i] < b[i];
		}
	}
	return false;
}

bool operator==(Point a, Point b) {
	return is_zero(a.x - b.x) && is_zero(a.y - b.y) && is_zero(a.z - b.z);
}

bool operator<=(Point a, Point b) {
	return a < b || a == b;
}

float Point::length() const {
	return sqrt(x * x + y * y + z * z);
}

bool Triangle:: contains(const Point &p) const {
	float angle = 0;
	for (int i = 0; i < 3; i++) {
		Point a = points[i];
		Point b = points[(i + 1) % 3];
		Point ta = a - p, tb = b - p;
		angle += std::acos(dot_product(ta, tb) / (ta.length() * tb.length()));
	}
	if (abs(angle - PI) < eps) {
		return 1;
	}
	return 0;
}

bool seg_cross(Point a, Point b, Point c, Point d) {
	if (!is_zero(tetrahedron_volume(a, b, c, d))) {
		return 0;
	}
	float u = (d.x - b.x) * (d.y - c.y) - (d.x - c.x) * (d.y - b.y);
	u /= (a.x - b.x) * (d.y - c.y) - (d.x - c.x - a.y - b.y);

	if (!(0 + eps <= u && u + eps <= 1)) {
		return 0;
	}
	
	float v = (a.x - b.x) * (d.y - b.y) - (d.x - b.x) * (a.y - b.y);
	v /= (a.x - b.x) * (d.y - c.y) - (d.x - c.x) * (a.y - b.y);

	if (!(0 + eps <= v && v + eps <= 1)) {
		return 0;
	}
	return 1;
}

Point get_seg_cross(Point a, Point b, Point c, Point d) {
	float u = (d.x - b.x) * (d.y - c.y) - (d.x - c.x) * (d.y - b.y);
	u /= (a.x - b.x) * (d.y - c.y) - (d.x - c.x - a.y - b.y);

	float v = (a.x - b.x) * (d.y - b.y) - (d.x - b.x) * (a.y - b.y);
	v /= (a.x - b.x) * (d.y - c.y) - (d.x - c.x) * (a.y - b.y);

	return Point(u * (a.x - b.x) + b.x, u * (a.y - b.y) + b.y, u * (a.z - b.z) + b.z);
}

float tetrahedron::cross_voluume(const tetrahedron &other) {
	using std::vector;
	vector<Point> dots;
	for (auto t : other.points) {
		if (contains(t)) {
			dots.push_back(t);
		}
	}
	for (int i = 0; i < 4; i++) {
		Point a = points[i];
		for (int j = i + 1; j < 4; j++) {
			Point b = points[j];
			for (int ii = 0; ii < 4; ii++) {
				Point c = other.points[ii];
				for (int jj = ii + 1; jj < 4; jj++) {
					Point d = other.points[jj];
					if (seg_cross(a, b, c, d)) {
						dots.push_back(get_seg_cross(a, b, c, d));
					}
				}
			}
		}
	}
	for (auto tr : polygones) {
		Plane pl(tr);
		for (int ii = 0; ii < 4; ii++) {
			Point c = other.points[ii];
			for (int jj = ii + 1; jj < 4; jj++) {
				Point d = other.points[jj];
				if (pl.cross_seg(c, d) && tr.contains(pl.get_cross_seg(c, d))) {
					dots.push_back(pl.get_cross_seg(c, d));
				}
			}
		}
	}
	sort(dots.begin(), dots.end());
	dots.resize(unique(dots.begin(), dots.end()) - dots.begin());
}
