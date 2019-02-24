/*
 * Cylinder.cpp
 *
 *  Created on: Feb 22, 2019
 *      Author: alonbs
 */

#include "Cylinder.h"


using namespace glm;



bool Cylinder::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{
	// To find intersection between Ray and canonical cylinder, we need to solve the following equation:
	// 	Cylinder: x^2 + y^2 = R^2
	// 	Ray:    (o + d*t)
	// we must solve a quadratic equations the form of Ax^2 + Bx + C = 0, where:
	// 	A: d.x^2 + d.y^2
	//  B: 2*o.x*d.x + 2*o.y*d.y
	// 	C: o.x^2 + o.y^2 - r^2

	Ray tr = this->invTransform() * r; // Transformed ray
	GLfloat A, B, C;
	GLfloat discriminant, disc_root;
	GLfloat t1 = INFINITY, t2 = INFINITY, t3 = INFINITY, t4 = INFINITY, t = INFINITY;
	vec3    ip, ip2; // Intersection points

	bool single_intersection = false;

	A = (tr.direction.x*tr.direction.x) + (tr.direction.y*tr.direction.y);
	B = (2*tr.origin.x*tr.direction.x) + (2*tr.origin.y*tr.direction.y);
	C = (tr.origin.x*tr.origin.x) + (tr.origin.y*tr.origin.y) - (radius * radius);

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
		t = t1;
	}
	else if (t1 > EPSILON && t2 > EPSILON) {
		// Both positive
		single_intersection = false;
	}
	else {
		// One positive, one negative
		// or 2 negatives
		single_intersection = true;
		t = glm::max(t1, t2);
	}

	if (t < 0) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}

	if (single_intersection) {

		ip = tr.origin + t * tr.direction;
		if (ip.z > maxCap || ip.z < minCap){
			return false;
		}

	}
	else { // 2 intersections

		GLfloat t_min, t_max;
		t_min = glm::min(t1,t2);
		t_max = glm::max(t1,t2);

		ip  = tr.origin + t_min * tr.direction;
		ip2 = tr.origin + t_max * tr.direction;

		if ((ip.z > maxCap && ip2.z > maxCap) || (ip.z < minCap && ip2.z < minCap) ){ // both intersection are on infinite cylinder (behind min or beyond max caps)
			return false;
		}

		if ((ip.z < minCap && ip2.z > minCap) || (ip.z > minCap && ip2.z < minCap)) { // min cap intersection
			t3 = (minCap - tr.origin.z) / tr.direction.z;
		}

		if ((ip.z < maxCap && ip2.z > maxCap) || (ip.z > maxCap && ip2.z < maxCap)) { // max cap intersection
			t4 = (maxCap - tr.origin.z) / tr.direction.z;
		}

		t_min = glm::min(t_min, glm::min(t3,t4));
		ip  = tr.origin + t_min * tr.direction;
	}


	// This is the normal at intersection point. (The Cylinder is aligned with the z-axis)
	vec3 n = vec3(ip - vec3(0,0, ip.z));
	n = normalize(vec3(mat3(this->invTransposeTrans()) * n));

	// M * p - to transform point back
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
		uv.x = acos(ip.x / radius) / (2*PI);
		//uv.x = (atan2(ip.x, ip.y) + PI) / (2*PI);
		//uv.y = 0.5 + 0.5 * n.z / (-minCap + maxCap);
		uv.y = (ip.z) / (-minCap + maxCap);

		uv = glm::clamp(uv,0.f, 1.f);


		printVec2("UV", uv);

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}
	if (properties) {
		*properties = this->properties();
	}
	return true;

}


void Cylinder::print() const
{
	std::cout << "Cylinder Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << " | minCap: " << minCap << " | Max Cap: " << maxCap << std::endl;
	Object::print();
}

