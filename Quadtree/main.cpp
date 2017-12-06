#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <vector>
#include "physical_quadtree.h"
#include "geometry.h"

using namespace std;

int main() {
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	vector<PObject> objects;
	int n, m;
	cin >> n >> m;
	vector<Point> points(n);
	for (int i = 0; i < n; i++) {
		cin >> points[i].x >> points[i].y >> points[i].z;
	}
	vector<vector<int> > connections(m, vector<int>(3));
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < 3; j++) {
			cin >> connections[i][j];
			connections[i][j]--;
		}
	}
	Object object(points, connections);
	int MAX = 10;
	Box limit(Point(-MAX, -MAX, -MAX), Point(MAX, MAX, MAX));
	objects.push_back(PObject(object, 5));
	Physical_quadtree quadtree(objects, limit);
	auto zone = quadtree.get_zones();
	cout << zone.size() << '\n';
	for (auto v : zone) {
		auto s = (v.second + v.first) / 2;
		auto d = (v.second - v.first) / 2;
		cout << s.x << ' ' << s.y << ' ' << s.z << ' ' << d.x << ' ' << d.y << ' ' << d.z << '\n';
	}
	int T = MAX * MAX * MAX;
	cout << T << '\n';
	for (int i = 0; i < MAX; i++) {
		for (int j = 0; j < MAX; j++) {
			for (int k = 0; k < MAX; k++) {
				int ok = !quadtree.is_empty_point(Point(i, j, k));
				cout << i << ' ' << j << ' ' << k << ' ' << ok << '\n';
			}
		}
	}
}
