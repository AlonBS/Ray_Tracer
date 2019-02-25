/*
 * Ray.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#include "Ray.h"

using namespace glm;

void Ray::_computeInverse()
{
	invDirection = 1 / direction;
	sign[0] = (invDirection.x < 0);
	sign[1] = (invDirection.y < 0);
	sign[2] = (invDirection.z < 0);
}

Ray::Ray()
:origin(vec3(0.0f, 0.0f, 0.0f)), direction(vec3(0.0f, 0.0f, 0.0f))
{
	_computeInverse();
}

Ray::Ray(vec3& origin, vec3& direction)
:origin(origin), direction(normalize(direction))
{
	_computeInverse();
	// TODO - consider changing origin a bit - due to floating point errors
}

Ray::Ray(const Ray& otherRay)
:origin(otherRay.origin), direction(otherRay.direction)
{
	_computeInverse();
}


Ray::~Ray() {
	// TODO Auto-generated destructor stub
}


Ray operator* (const mat4& M, const Ray& r)
{
	vec3 transOrig = vec3(M * vec4(r.origin, 1.0f));
	vec3 transDir  = vec3(M * vec4(r.direction, 0.0f));
	return Ray(transOrig, transDir);
}


