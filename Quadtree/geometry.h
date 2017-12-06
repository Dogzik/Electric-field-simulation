#pragma once
#include <vector>

float const PI = std::acos(-1);
const float eps = 1e-3f;

bool is_zero(float x);

struct Point {
	union {
		struct {
			float x, y, z;
		};
		float data[3];
	};

	Point(float _x, float _y, float _z);
	Point();
	Point(Point const& b) : Point(b.x, b.y, b.z) { }

	friend Point operator+(Point a, Point b);
	friend Point operator-(Point a, Point b);

	float& operator[](size_t t);
	float operator[](size_t t) const;

	friend bool operator<(Point a, Point b);
	friend bool operator==(Point a, Point b);
	friend bool operator<=(Point a, Point b);

	Point operator/(float c) const;
	Point operator*(float c) const;
	Point operator-=(Point b);
	float length() const;
};

Point cross_product(Point a, Point b);
float dot_product(Point a, Point b);

struct Triangle {
	typedef Point* iterator;
	union {
		struct {
			Point a, b, c;
		};

		Point points[3];
	};

	Triangle(Point a, Point b, Point c);
	Triangle(Triangle const& b);

	iterator begin();
	iterator end();
	Point operator[](size_t x);
	bool contains(const Point &p) const;
};

struct Plane {
	float a, b, c, d;
	Plane(const Point &p1, const Point &p2, const Point &p3) {
		a = p1.y * (p2.z - p3.z) + p2.y * (p3.z - p1.z) + p3.y * (p1.z - p2.z);
		b = p1.z * (p2.x - p3.x) + p2.z * (p3.x - p1.x) + p3.z * (p1.x - p2.x);
		c = p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y);
		d = -(p1.x * (p2.y * p3.z - p3.y * p2.z) + p2.x * (p3.y * p1.z - p1.y * p3.z) + p3.x * (p1.y * p2.z - p2.y * p1.z));
	}
	Plane(Triangle const &a) : Plane(a.a, a.b, a.c) {}
	float unnorned_dist(const Point &p) const{
		return (p.x * a + p.y * b + p.z * c + d);
	}
	bool cross_seg(const Point &p1, const Point &p2) const {
		return (unnorned_dist(p1) * unnorned_dist(p2) + eps < 0);
	}
	Point get_cross_seg(const Point &p1, const Point &p2) const {
		float x = -(b * p1.x * p2.y - b * p2.x * p1.y + c * p1.x * p2.z - c * p2.x * p1.z + d * p1.x - d * p2.x) / (a * p1.x - a * p2.x + b * p1.y - b * p2.y + c * p1.z - c * p2.z);
		float y = p1.y + (p2.y - p1.y) * (x - p1.x) / (p2.x - p1.x);
		float z = p1.z + (p2.z - p1.z) * (x - p1.x) / (p2.x - p1.x);
		return Point(x, y, z);
	}
};

bool seg_cross(Point a, Point b, Point c, Point d);
Point get_seg_cross(Point a, Point b, Point c, Point d);

struct Interval {
	float min;
	float max;
};

Interval get_interval(Triangle triangle, Point axis);
bool overlap_on_axis(Triangle t1, Triangle t2, Point axis);
bool cross_triangle_triangle(Triangle t1, Triangle t2);

struct Box {
	union {
		struct {
			Point first, second;
		};
		Point data[2];
	};

	Box(Point first, Point second);
	Box(Box const& b) {
		data[0] = b.data[0];
		data[1] = b.data[1];
	}
	bool contains(Point x) const;
	std::vector<Point> get_points() const;
};

float tetrahedron_volume(Point a, Point b, Point c, Point d);
float tetrahedron_volume(Point a, Triangle tr);

enum CrossType {
	LIMIT_IN_OBJ,
	OBJ_IN_LIMIT,
	INTERSECTION,
	EMPTY_INTERSECTION
};

class Object {
protected:
	std::vector<Triangle> polygones;
	std::vector<Point> points;
	float real_volume;

	void init();
public:
	typedef std::vector<Triangle>::iterator iterator;
	iterator begin();
	iterator end();

	Object(const std::vector<Point>& points, std::vector<std::vector<int> > connect);
	Object(const std::vector<Triangle>& trianguals);
	Object(Box trianguals);

	bool contains(Point p);
	CrossType cross(Box limit);
};

struct tetrahedron : public Object {
	float cross_voluume(const tetrahedron &other);
};
