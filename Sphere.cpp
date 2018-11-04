/*
 * Sphere.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#include "Sphere.h"


using namespace glm;



bool Sphere::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
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
	GLfloat x1, x2, x;
	vec3    intersection_point;


	A = glm::dot(tr.direction, tr.direction);
	B = 2 * glm::dot(tr.direction, (tr.origin - center));
	C = glm::dot((tr.origin - center), (tr.origin - center)) - (radius * radius);

	discriminant = B*B - 4*A*C;
	if (discriminant < 0) {
		// No intersection
		return false;
	}

	disc_root = glm::sqrt(discriminant);
	x1 = (-B + disc_root) / (2*A);
	x2 = (-B - disc_root) / (2*A);

	if (abs(x1 - x2) < EPSILON) {
		// same solution
		x = x1;
	}
	else if (x1 > EPSILON && x2 > EPSILON) {
		// Both positive
		x = glm::min(x1, x2);
	}
	else {
		// One positive, one negative
		// or 2 negatives
		x = glm::max(x1, x2);
	}

	if (x < 0) {
		// If dist is a negative values (accounting for floating point errors)
		// then both solutions were negative. Meaning we have to go back from the origin of
		// the ray (against its direction) to the intersection point - which means of course that
		// there's no intersection.
		return false;
	}


	intersection_point = tr.origin + x * tr.direction;
	vec3 n = vec3(intersection_point - center);

	// M * p - to transform point back
	intersection_point = vec3(this->transform() * vec4(intersection_point, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	dist = length(intersection_point - r.origin);

	if (point) {
		*point = intersection_point;
	}
	if (normal) {
		*normal = normalize(vec3(mat3(this->invTransposeTrans()) * n));
	}
	if (texColors) {

		vec3 d = normalize(intersection_point - center);
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


const void Sphere::print() const
{
	std::cout << "Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << std::endl;
	Object::print();
}

