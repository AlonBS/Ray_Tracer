/*
 * Cylinder.cpp
 *
 *  Created on: Feb 22, 2019
 *      Author: alonbs
 */

#include <vector>
#include "Cylinder.h"

using namespace glm;



bool Cylinder::intersectsRay(
		const Ray &r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties) const
{
	// To find intersection between Ray and canonical cylinder (aligned to the y-axis), we need to solve the following equation:
	// 	Cylinder: x^2 + z^2 = R^2
	// 	Ray:    (o + d*t)
	// we must solve a quadratic equations the form of Ax^2 + Bx + C = 0, where:
	// 	A: d.x^2 + d.z^2
	//  B: 2*o.x*d.x + 2*o.z*d.z
	// 	C: o.x^2 + o.z^2 - r^2

	Ray tr = this->_transforms._invTransform * r; // Transformed ray
	GLfloat A, B, C;
	GLfloat discriminant, disc_root;
	GLfloat t_min = INFINITY, t_max = INFINITY;
	GLfloat t1 = INFINITY, t2 = INFINITY, t3 = INFINITY, t4 = INFINITY;
	vec3    ip, ip2; // Intersection points

	bool single_intersection = false;
	bool minCapIntersection = false, maxCapIntersection = false;

	updateStats(INCREMENT_INTERSECTION_TESTS_COUNT);

	A = (tr.direction.x*tr.direction.x) + (tr.direction.z*tr.direction.z);
	B = (2*tr.origin.x*tr.direction.x) + (2*tr.origin.z*tr.direction.z);
	C = (tr.origin.x*tr.origin.x) + (tr.origin.z*tr.origin.z) - (radius * radius);

	discriminant = (B*B) - 4*A*C;
	if (discriminant < 0) {
		// No intersection
		return false;
	}


	disc_root = glm::sqrt(discriminant);
	t1 = (-B + disc_root) / (2*A);
	t2 = (-B - disc_root) / (2*A);
	if (abs(t1 - t2) < EPSILON) {
		// same solution
		single_intersection = true;
		t_min = t1;
	}
	else if (t1 > EPSILON && t2 > EPSILON) {
		// Both positive
		single_intersection = false;
	}
	else {
		// One positive, one negative
		// or 2 negatives
		single_intersection = true;
		t_min = glm::max(t1, t2);
	}

	if (t_min < 0) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}

	if (single_intersection) {

		ip = tr.origin + t_min * tr.direction;
		if (ip.y > maxCap || ip.y < minCap){
			return false;
		}
	}

	else { // 2 intersections

		t_min = glm::min(t1,t2);
		t_max = glm::max(t1,t2);

		ip  = tr.origin + t_min * tr.direction;
		ip2 = tr.origin + t_max * tr.direction;

		if ((ip.y > maxCap && ip2.y > maxCap) || (ip.y < minCap && ip2.y < minCap) ){ // both intersection are on infinite cylinder (behind min or beyond max caps)
			return false;
		}
		if ((ip.y < minCap && ip2.y > minCap) || (ip.y > minCap && ip2.y < minCap)) { // min cap intersection
			t3 = (minCap - tr.origin.y) / tr.direction.y;
		}

		if ((ip.y < maxCap && ip2.z > maxCap) || (ip.y > maxCap && ip2.y < maxCap)) { // max cap intersection
			t4 = (maxCap - tr.origin.y) / tr.direction.y;
		}
		t_min = glm::min(t_min, glm::min(t3,t4));

		/* This is so we can properly display the object at the caps, as the minimum no longer applies here */
		if (ip.y < minCap) {
			t_min = t3;
			minCapIntersection = true;
		}
		else if (ip.y > maxCap) {
			t_min = t4;
			maxCapIntersection = true;
		}
		ip  = tr.origin + t_min * tr.direction;
	}


	// This is the normal at intersection point. (The Cylinder is aligned with the y-axis)
	vec3 n = _normalAt(ip, minCapIntersection, maxCapIntersection);
	vec3 N = n;
	n = normalize(this->_transforms._invTransposeTrans * n);

	// M * p - to transform point back
	ip2 = ip; // for texture - we need the non transformed position
	ip = vec3(this->_transforms._transform * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	*dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {
		vec2 uv;
		uv.x = 0.5 + atan2(ip2.x, ip2.z) / (2*PI); // == (atan2(ip2.x, ip2.z) + PI) / (2*PI);
		uv.y = (ip2.y - minCap) / (maxCap - minCap); // ==  0.5 + ip2.y / (maxCap - minCap) - because maxCap = -minCap;

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);

		if (!_objectGlobalProperties.no_bump_maps && _normalMap) {
			vec3 T = normalize(cross(vec3(0,1,0), N));
			*normal = __GetTBNAndNorm(N, T, uv); // We need the original normal, not the transformed one.
		}

	}
	if (properties) {
		*properties = this->_properties;
	}


	updateStats(INCREMENT_HITS_COUNT);
	return true;

}


vec3 Cylinder::_normalAt(const vec3& point, bool minCapIntersect, bool maxCapIntersect) const
{
	if (minCapIntersect)
		return vec3 (0, -1, 0);
	else if (maxCapIntersect)
		return vec3 (0, 1, 0);
	else
		return vec3(point - vec3(0,point.y, 0));
}


void Cylinder::print() const
{
	std::cout << "Cylinder Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << " | minCap: " << minCap << " | Max Cap: " << maxCap << std::endl;
	Object::print();
}




void Cylinder::computeBoundingBox()
{
	GLfloat min = -radius;
	GLfloat max = +radius;

	vec3 minBound = vec3(+INFINITY, +INFINITY, +INFINITY);
	vec3 maxBound = vec3(-INFINITY, -INFINITY, -INFINITY);

	std::vector<vec3> verts {
		vec3(min, minCap, min),
		vec3(min, minCap, max),
		vec3(min, maxCap, min),
		vec3(min, maxCap, max),
		vec3(max, minCap, min),
		vec3(max, minCap, max),
		vec3(max, maxCap, min),
		vec3(max, maxCap, max),
	};


	for (vec3& v : verts) {

		v = vec3 (this->_transforms._transform * vec4(v, 1.0f));;

		minBound = glm::min(v, minBound);
		maxBound = glm::max(v, maxBound);
	}

	this->bbox = make_unique<AABB>(minBound, maxBound);
}

