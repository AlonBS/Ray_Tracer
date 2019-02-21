/*
 * Sphere.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#include "Cylinder.h"


using namespace glm;



bool Cylinder::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{
	// To find intersection between Ray and Sphere represented the following way:
	// 	Sphere: (P - C )^2 - r^2 = 0
	// 	Ray:    (o + d*x)
	// we must solve a quadratic equations the form of Ax^2 + Bx + C = 0, where:
	// 	A: d^2
	//  B: 2 * d * (o - c)
	// 	C: (o -c)^2 - r^2


	Ray tr = this->invTransform() * r; // Transformed ray
	GLfloat A, B, C;
	GLfloat discriminant, disc_root;
	GLfloat t1, t2, t3, t4, t;
	vec3    ip, ip2; // Intersection points


	A = (tr.direction.x*tr.direction.x) + (tr.direction.y*tr.direction.y);
	B = (2*tr.origin.x*tr.direction.x) + (2*tr.origin.y*tr.direction.y);
	C = (tr.origin.x*tr.origin.x) + (tr.origin.y*tr.origin.y) - 1;

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
		t = t1;
	}
	else if (t1 > EPSILON && t2 > EPSILON) {
		// Both positive
		t = glm::min(t1, t2);
		ip2 = tr.origin + glm::max(t1,t2) * tr.direction;
	}
	else {
		// One positive, one negative
		// or 2 negatives
		t = glm::max(t1, t2);
	}

	if (t < 0) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}


	ip = tr.origin + t * tr.direction;
	if (ip.z <  -1 && ip2.z > -1) { // Min cap intersection
		t3 = (-1 - tr.origin.z) / tr.direction.z;
	}
	if (ip.z <  1 && ip2.z > 1) { // Max cap intersection
		t4 = (1 - tr.origin.z) / tr.direction.z;
	}


	vec3 n = vec3(ip - center);

	// M * p - to transform point back
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = normalize(vec3(mat3(this->invTransposeTrans()) * n));
	}
	if (texColors) {

		vec3 d = normalize(ip - center);
		vec2 uv;
		uv.x = 0.5 + atan2(d.x, d.z) / (2 * PI);
		uv.y = 0.5 + 0.5 * d.y;
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
	std::cout << "Cylinder Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << std::endl;
	Object::print();
}

