/*
 * Sphere.h
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#ifndef CYLINDER_H
#define CYLINDER_H

#include <GL/glew.h>
#include <GL/glut.h>


#include "Object.h"


class Cylinder: public Object {

private:

	glm::vec3 center;
	GLfloat radius;

	GLfloat minCap, maxCap;

	vec3 __calculateTexture(vec3& p);


public:

	Cylinder(const glm::vec3 & center, GLfloat &radius)
	: Object(), center(center), radius(radius)
	{
		minCap = -5;
		maxCap = 5;

	}
	virtual ~Cylinder() {}

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* CYLINDER_H */
