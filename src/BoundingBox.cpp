#include "BoundingBox.h"

using namespace glm;

bool BoundingBox::intersectsRay(const Ray& tr, GLfloat* t_near)
{
	GLfloat tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[tr.sign[0]].x - tr.origin.x) * tr.invDirection.x;
	tmax = (bounds[1-tr.sign[0]].x - tr.origin.x) * tr.invDirection.x;
	tymin = (bounds[tr.sign[1]].y - tr.origin.y) * tr.invDirection.y;
	tymax = (bounds[1-tr.sign[1]].y - tr.origin.y) * tr.invDirection.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin) {
		tmin = tymin;
	}
	if (tymax < tmax) {
		tmax = tymax;
	}

	tzmin = (bounds[tr.sign[2]].z - tr.origin.z) * tr.invDirection.z;
	tzmax = (bounds[1-tr.sign[2]].z - tr.origin.z) * tr.invDirection.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin) {
		tmin = tzmin;
	}
	if (tzmax < tmax) {
		tmax = tzmax;
	}

	if (tmin < 0 || isnan(tmin)) {
		tmin = tmax;
		if (tmin < 0 || isnan(tmin)) {
			//			 If dist is a negative values (accounting for floating point errors)
			//			 then both solutions were negative. Meaning we have to go back from the origin of
			//			 the ray (against its direction) to the intersection point - which means of course that
			//			 there's no intersection.
			return false;
		}
	}

	*t_near = tmin;
	return true;
}


void BoundingBox::print() const
{
	std::cout << "Min Bound: ("<< bounds[0].x << "," << bounds[0].y << "," << bounds[0].z << ")" << std::endl;
	std::cout << "Max Bound: ("<< bounds[1].x << "," << bounds[1].y << "," << bounds[1].z << ")" << std::endl;
}



