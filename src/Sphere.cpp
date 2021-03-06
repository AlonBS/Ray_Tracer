/*
 * Sphere.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#include <vector>
#include "Sphere.h"


using namespace glm;



bool Sphere::intersectsRay(
		const Ray &r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties) const
{
	// To find intersection between Ray and Sphere represented the following way:
	// 	Sphere: (P - C )^2 - r^2 = 0
	// 	Ray:    (o + d*t)
	// we must solve a quadratic equations the form of At^2 + Bt + C = 0, where:
	// 	A: d^2
	//  B: 2 * d * (o - c)
	// 	C: (o -c)^2 - r^2

	Ray tr = this->_transforms._invTransform * r; // Transformed ray
	GLfloat A, B, C;
	GLfloat discriminant, disc_root;
	GLfloat t1, t2, t = INFINITY;
	vec3    intersection_point;


	updateStats(INCREMENT_INTERSECTION_TESTS_COUNT);


	A = glm::dot(tr.direction, tr.direction);
	B = 2 * glm::dot(tr.direction, (tr.origin - center));
	C = glm::dot((tr.origin - center), (tr.origin - center)) - (radius * radius);

	discriminant = B*B - 4*A*C;
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


	intersection_point = tr.origin + t * tr.direction;
	// The normal at intersection point (to the canonical sphere)
	vec3 n = vec3(intersection_point - center);
	vec3 N = n;
	// The normal transformation fix
	n = normalize(this->_transforms._invTransposeTrans * n);


	// M * p - to transform point back
	intersection_point = vec3(this->_transforms._transform * vec4(intersection_point, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	*dist = length(intersection_point - r.origin);

	if (point) {
		*point = intersection_point;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {

		vec2 uv;
		uv.x = 0.5 + atan2(n.x, n.z) / (2*PI);
		uv.y = 0.5 + 0.5 * n.y;

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);

		if (!_objectGlobalProperties.no_bump_maps && _normalMap) {

			// note that around the polars, we'll get in correct results. But this is so rarely happens and when tested
			// it didn't show any visual effect (especially when anti-aliasing). So I chose to left this out
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


void Sphere::print() const
{
	std::cout << "Center: (" << center.x << "," << center.y << "," << center.z << ") | Radius: " << radius << std::endl;
	Object::print();
}


void Sphere::computeBoundingBox()
{
	GLfloat min = -radius;
	GLfloat max = +radius;

	vec3 minBound = vec3(+INFINITY, +INFINITY, +INFINITY);
	vec3 maxBound = vec3(-INFINITY, -INFINITY, -INFINITY);

	std::vector<vec3> verts {
		vec3(min, min, min),
		vec3(min, min, max),
		vec3(min, max, min),
		vec3(min, max, max),
		vec3(max, min, min),
		vec3(max, min, max),
		vec3(max, max, min),
		vec3(max, max, max),
	};


	for (vec3& v : verts) {

		v = vec3 (this->_transforms._transform * vec4(v, 1.0f));;

		minBound = glm::min(v, minBound);
		maxBound = glm::max(v, maxBound);
	}

	this->bbox = make_unique<AABB>(minBound, maxBound);

}

