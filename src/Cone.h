/*
 * Cone.h
 *
 *  Created on: Feb 27, 2018
 *      Author: alonbs
 */

#ifndef CONE_H
#define CONE_H

#include <GL/glew.h>
#include <GL/glut.h>


#include "Object.h"


class Cone: public Object {

private:

	glm::vec3 center;
	GLfloat minCap, maxCap;

	GLfloat height;
	GLfloat radius;


	vec3 _normalAt(const vec3& point, bool minCapIntersect, bool maxCapIntersect) const;

public:

	Cone(const ObjectProperties& properties, const ObjectTransforms& transforms,
		 const glm::vec3 & center, GLfloat& minCap, GLfloat& maxCap)
	: Object(properties, transforms),
	  center(center), minCap(minCap), maxCap(maxCap), height(maxCap - minCap)
	{
		radius = glm::max(abs(maxCap), abs(minCap)); // TODO - consider
	}

	virtual ~Cone() {}

	virtual bool intersectsRay(
			const Ray& r,
			GLfloat* dist,
			vec3* point,
			vec3* normal,
			ObjectTexColors* texColors,
			ObjectProperties* properties) const;

	virtual void print() const;

	virtual void computeBoundingBox();


};

#endif /* CONE_H */
