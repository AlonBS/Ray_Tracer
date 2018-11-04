/*
 * Ray.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#include "Ray.h"

using namespace glm;

Ray::Ray()
:origin(vec3(0.0f, 0.0f, 0.0f)), direction(vec3(0.0f, 0.0f, 0.0f))
{

}

Ray::Ray(vec3& origin, vec3& direction)
:origin(origin), direction(normalize(direction))
{

	// TODO - consider changing origin a bit - due to floating point errors
}

Ray::Ray(const Ray& otherRay)
:origin(otherRay.origin), direction(otherRay.direction)
{

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


