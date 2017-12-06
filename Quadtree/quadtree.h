#pragma once
#include <vector>
#include "geometry.h"
#include "physical_geometry.h"

enum NodeType {
	EMPTY_NODE = 1,
	FULL_NODE = 2,
	NO_EMPTY_NODE = 3
};

//template<class T>
//class Data_example<T> {
//public:
//	T get_value(std::vector<PObject const*>);
//	T merge(T const& a, T const& b);
//};

template <class T>
class Quadtree {
	const int MAX_H = 16;

	struct node {
		node(node* left, node* right, Box limit, int height, NodeType full_empty);
		node(node* left, node* right, Box limit, int height);
		node(Box limit, int height, NodeType full_empty, T data);

		node* left;
		node* right;
		Box limit;
		int height;
		NodeType type;
		T data;
	};

	std::vector<PObject> objects;
	std::vector<Box> zones;

	NodeType test_for_in_out(Box limit);
	void add_zone(Box limit);

	node* dfs(Box limit, int height);
	void clear_dfs(node* cur);

	std::vector<PObject const*> get_all_intersection(Box limit);
	static NodeType get_type(node* v);

protected:
	node* get(Point t, node* cur, int height) const;
	static T get_data(node* v);
	node* root;

public:
	Quadtree(std::vector<PObject> const& objects_, Box limit);
	~Quadtree();
	void clear();

	bool is_empty_point(Point p) const;
	float get_data(Point p) const;

	std::vector<Box> get_zones() const;
};

inline std::pair<Box, Box> divide_box(int h, Box limit) {
	Point s = (limit.first + limit.second) / 2;
	Box a = limit;
	Box b = limit;
	int cur_d = h % 3;
	a.first[cur_d] = s[cur_d];
	b.second[cur_d] = s[cur_d];
	return std::make_pair(a, b);
}

template <class T>
Quadtree<T>::node::node(node* left, node* right, Box limit, int height, NodeType full_empty) :
	left(left),
	right(right),
	limit(limit),
	height(height),
	type(full_empty),
	data() { }

template <class T>
Quadtree<T>::node::node(node* left, node* right, Box limit, int height) :
	left(left),
	right(right),
	limit(limit),
	height(height) {
	type = static_cast<NodeType>(get_type(left) | get_type(right));
	data = T::merge(get_data(left), get_data(right));
}

template <class T>
Quadtree<T>::node::node(Box limit, int height, NodeType full_empty, T data) :
	left(nullptr),
	right(nullptr),
	limit(limit),
	height(height),
	type(full_empty),
	data(data) { }


template <class T>
NodeType Quadtree<T>::test_for_in_out(Box limit) {
	bool ok[4] = {};
	for (auto obj : objects) {
		ok[obj.cross(limit)] = true;
	}
	if (ok[LIMIT_IN_OBJ]) {
		return FULL_NODE;
	}
	if (ok[OBJ_IN_LIMIT] | ok[INTERSECTION]) {
		return NO_EMPTY_NODE;
	}
	return EMPTY_NODE;
}

template <class T>
void Quadtree<T>::add_zone(Box limit) {
	zones.push_back(limit);
}

template <class T>
typename Quadtree<T>::node* Quadtree<T>::dfs(Box limit, int height = 0) {
	NodeType temp = test_for_in_out(limit);
	if (temp == FULL_NODE) {
		add_zone(limit);
		return new node(
			limit,
			height,
			FULL_NODE,
			T::get_value(get_all_intersection(limit), limit)
		);
	}
	if (temp == EMPTY_NODE || height > MAX_H) {
		return nullptr;
	}
	auto boxs = divide_box(height, limit);
	auto left = dfs(boxs.first, height + 1);
	auto right = dfs(boxs.second, height + 1);
	if (get_type(left) == get_type(right)
		&& get_type(left) == EMPTY_NODE) {
		clear_dfs(left);
		clear_dfs(right);
		return nullptr;
	}
	return new node(
		left,
		right,
		limit,
		height
	);
}

template <class T>
typename Quadtree<T>::node* Quadtree<T>::get(Point t, node* cur, int height = 0) const {
	if (cur == nullptr) {
		return nullptr;
	}
	if (cur->type != NO_EMPTY_NODE) {
		return cur;
	}
	auto boxs = divide_box(height, cur->limit);
	if (boxs.first.contains(t)) {
		return get(t, cur->left, height + 1);
	}
	return get(t, cur->right, height + 1);
}

template <class T>
void Quadtree<T>::clear_dfs(node* cur) {
	if (cur != nullptr) {
		clear_dfs(cur->left);
		clear_dfs(cur->right);
		delete cur;
	}
}

template <class T>
std::vector<PObject const*> Quadtree<T>::get_all_intersection(Box limit) {
	std::vector<PObject const*> result;
	for (auto obj : objects) {
		auto t = obj.cross(limit);
		if (t != EMPTY_INTERSECTION) {
			result.push_back(&obj);
		}
	}
	return result;
}

template <class T>
T Quadtree<T>::get_data(node* v) {
	if (v == nullptr) {
		return T();
	}
	return v->data;
}

template <class T>
NodeType Quadtree<T>::get_type(node* v) {
	if (v == nullptr) {
		return EMPTY_NODE;
	}
	return v->type;
}

template <class T>
Quadtree<T>::Quadtree(std::vector<PObject> const& objects_, Box limit) : objects(objects_) {
	root = dfs(limit, 0);
}

template <class T>
Quadtree<T>::~Quadtree() {
	clear();
}

template <class T>
void Quadtree<T>::clear() {
	clear_dfs(root);
	root = nullptr;
}

template <class T>
bool Quadtree<T>::is_empty_point(Point p) const {
	return get_type(get(p, root, 0)) == EMPTY_INTERSECTION;
}

template <class T>
float Quadtree<T>::get_data(Point p) const {
	return get_data(get(p, root, 0));
}

template <class T>
std::vector<Box> Quadtree<T>::get_zones() const {
	return zones;
}
