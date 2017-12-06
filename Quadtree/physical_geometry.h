#pragma once
#include "geometry.h"

class PObject : public Object {
	float charge;
public:
	PObject(const std::vector<Point>& points, std::vector<std::vector<int> > connect, float charge);
	PObject(const std::vector<Triangle>& trianguals, float charge);
	PObject(Box trianguals, float charge);
	PObject(Object const& object, float charge);

	float get_charge() const;
};
