/*
 * Cylinder.cpp
 *
 *  Created on: Feb 22, 2019
 *      Author: alonbs
 */

#include "Box.h".h"


using namespace glm;



bool Box::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{
	// To find intersection between Ray and canonical cylinder (aligned to the y-axis), we need to solve the following equation:
	// 	Cylinder: x^2 + z^2 = R^2
	// 	Ray:    (o + d*t)
	// we must solve a quadratic equations the form of Ax^2 + Bx + C = 0, where:
	// 	A: d.x^2 + d.z^2
	//  B: 2*o.x*d.x + 2*o.z*d.z
	// 	C: o.x^2 + o.z^2 - r^2

	Ray tr = this->invTransform() * r; // Transformed ray
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin = (bounds[tr.sign[0]].x - tr.origin.x) * tr.invDirection.x;
	tmax = (bounds[1-tr.sign[0]].x - tr.origin.x) * tr.invDirection.x;
	tymin = (bounds[r.sign[1]].y - r.origin.y) * tr.invDirection.y;
	tymax = (bounds[1-r.sign[1]].y - r.origin.y) * tr.invDirection.y;

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	tzmin = (bounds[r.sign[2]].z - r.origin.z) * r.invDirection.z;
	tzmax = (bounds[1-r.sign[2]].z - r.origin.z) * r.invDirection.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	return true;



	if (tmin < 0) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}




	ip  = tr.origin + t_min * tr.direction;


	// This is the normal at intersection point. (The Cylinder is aligned with the y-axis)
	vec3 n = vec3(ip - vec3(0,ip.y, 0));
	n = normalize(vec3(mat3(this->invTransposeTrans()) * n));

	// M * p - to transform point back
	ip2 = ip; // for texture - we need the non transformed position
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {

		vec2 uv;
		uv.x = (atan2(ip2.x, ip2.z) + PI) / (2*PI);
		uv.y = 0.5 + (ip2.y) / (maxCap - minCap);
//		uv = glm::clamp(uv,0.f, 1.f);

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}
	if (properties) {
		*properties = this->properties();
	}
	return true;

}


void Box::print() const
{
	std::cout << "Min Bound: ("<< bounds[0].x << "," << bounds[0].y << "," << bounds[0].z << ")" << endl;
	std::cout << "Max Bound: ("<< bounds[1].x << "," << bounds[1].y << "," << bounds[1].z << ")" << endl;
	Object::print();
}

