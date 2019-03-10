#include "Box.h"


using namespace glm;



bool Box::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{

	Ray tr = this->invTransform() * r; // Transformed ray
	GLfloat tmin, tmax, tymin, tymax, tzmin, tzmax;
	vec3 ip{}, ip2{};
	vec3 n;

	++rayTracerStats.numOfIntersectTests;

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


	ip  = tr.origin + tmin * tr.direction;
	n = _normalAt(ip);
	// Normal transformation
	n = normalize(vec3(mat3(this->invTransposeTrans()) * n));

	// M * p - to transform point back
	ip2 = ip;
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



