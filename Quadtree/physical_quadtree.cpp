#include "physical_quadtree.h"
#include "physical_geometry.h"

Phy_node::Phy_node() :
	charge_point(),
	sum_charge(0) { }

Point Phy_node::get_point() const {
	return charge_point;
}

float Phy_node::get_charge() const {
	return sum_charge;
}

Phy_node::Phy_node(Point charge_point, float sum_charge) :
	charge_point(charge_point),
	sum_charge(sum_charge) { }

Phy_node Phy_node::get_value(std::vector<PObject const*> const& objects, Box limit) {
	float sum = 0;
	for (auto object : objects) {
		sum += object->get_charge();
	}
	return Phy_node(
		(limit.first + limit.second) / 2 * sum,
		sum
	);
}

Phy_node Phy_node::merge(Phy_node const& a, Phy_node const& b) {
	return Phy_node(
		a.get_point() + b.get_point(),
		a.get_charge() + b.get_charge()
	);
}

Physical_quadtree::Physical_quadtree(std::vector<PObject> const& objects, const Box& limit)
	: Quadtree<Phy_node>(objects, limit) { }

float Physical_quadtree::get_charge(Point point) const {
	return get_data(get(point, root, 0)).get_charge();
}
