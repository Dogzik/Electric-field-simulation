#include "physical_geometry.h"

PObject::PObject(const std::vector<Point>& points, std::vector<std::vector<int> > connect, float charge) :
	Object(points, connect),
	charge(charge) { }

PObject::PObject(const std::vector<Triangle>& trianguals, float charge) :
	Object(trianguals),
	charge(charge) { }

PObject::PObject(Box trianguals, float charge) :
	Object(trianguals),
	charge(charge) { }

PObject::PObject(Object const& object, float charge) :
	Object(object),
	charge(charge) { }

float PObject::get_charge() const {
	return charge;
}
