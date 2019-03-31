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


	vec3 _normalAt(const vec3& point, bool minCapIntersect, bool maxCapIntersect);


public:

	Cone(const glm::vec3 & center, GLfloat& minCap, GLfloat& maxCap)
	: Object(), center(center), minCap(minCap), maxCap(maxCap), height(maxCap - minCap)
	{
//		radius = sqrt(glm::max(abs(maxCap), abs(minCap)));
		radius = glm::max(abs(maxCap), abs(minCap)); // TODO - consider
	}

	virtual ~Cone() {}

	virtual bool intersectsRay(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;

	void computeBoundingBox();
};

#endif /* CONE_H */
