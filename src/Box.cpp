#include "Box.h"


using namespace glm;



bool Box::intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{

	Ray tr = this->invTransform() * r; // Transformed ray
	GLfloat tmin;
	vec3 ip{}, ip2{};
	vec3 n;

	++rayTracerStats.numOfIntersectTests;

	// There's no need for a bounding box for a box. Still, we don't want code repetition, so we use this temp box.
	// Note we transform the ray
	AABB aabb{bounds[0], bounds[1]};
	if (!aabb.intersectsRay(tr, &tmin)) {
		return false;
	}

	ip  = tr.origin + tmin * tr.direction;
	n = _normalAt(ip);
	// Normal transformation
	n = normalize(this->invTransposeTrans() * n);

	// M * p - to transform point back
	ip2 = ip;
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	*dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {

		vec2 uv = _textureAt(ip2);
		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}
	if (properties) {
		*properties = this->properties();
	}

	++rayTracerStats.numOfHits;
	return true;
}

vec3 Box::_normalAt(const vec3 &point)
{

	GLfloat xmin = bounds[0].x;
	GLfloat xmax = bounds[1].x;
	GLfloat ymin = bounds[0].y;
	GLfloat ymax = bounds[1].y;
	GLfloat zmin = bounds[0].z;
	GLfloat zmax = bounds[1].z;

	/*
	 * Note that at the edges we get either of the normals, which is fine, as we don't
	 * care too much what is the normal at this point.
	 */
	if (abs(point.x - xmax) < EPSILON) {
		return vec3(1, 0, 0);
	}
	if (abs(point.x - xmin) < EPSILON) {
		return vec3(-1, 0, 0);
	}
	if (abs(point.y - ymax) < EPSILON) {
		return vec3(0, 1, 0);
	}
	if (abs(point.y - ymin) < EPSILON) {
		return vec3(0, -1, 0);
	}
	if (abs(point.z - zmax) < EPSILON) {
		return vec3(0, 0, 1);
	}
	if (abs(point.z - zmin) < EPSILON) {
		return vec3(0, 0, -1);
	}

	/* We should never get here */
	assert(true);
	return vec3(0,0,0);
}

vec2 Box::_textureAt(const vec3& point)
{
	vec2 uv {};

	if (abs(point.x - bounds[0].x) < EPSILON ||
	    abs(point.x - bounds[1].x) < EPSILON) { // on YZ plane

		uv.x = (point.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
		uv.y = (point.z - bounds[0].z) / (bounds[1].z - bounds[0].z);

	}
	else if (abs(point.y - bounds[0].y) < EPSILON ||
			 abs(point.y - bounds[1].y) < EPSILON) { // on XZ plane

		uv.x = (point.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
		uv.y = (point.z - bounds[0].z) / (bounds[1].z - bounds[0].z);
	}

	else { // on XY plane

		uv.x = (point.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
		uv.y = (point.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
	}

	// Again - we don't care too much about edges
	return uv;
}


void Box::print() const
{
	std::cout << "Min Bound: ("<< bounds[0].x << "," << bounds[0].y << "," << bounds[0].z << ")" << endl;
	std::cout << "Max Bound: ("<< bounds[1].x << "," << bounds[1].y << "," << bounds[1].z << ")" << endl;
	Object::print();
}



