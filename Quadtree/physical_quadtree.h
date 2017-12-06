#pragma once
#include <vector>
#include "geometry.h"
#include "quadtree.h"
#include "physical_geometry.h"

class Phy_node {
	Point charge_point;
	float sum_charge;

public:
	Phy_node();
	Point get_point() const;
	float get_charge() const;
	Phy_node(Point charge_point, float sum_charge);
	static Phy_node get_value(std::vector<PObject const*> const& objects, Box limit);
	static Phy_node merge(Phy_node const& a, Phy_node const& b);
};

class Physical_quadtree : public Quadtree<Phy_node> {
public:
	Physical_quadtree(std::vector<PObject> const& objects, Box const& limit);

	float get_charge(Point point) const;
};
