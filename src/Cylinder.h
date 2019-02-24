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
	GLfloat minCap, maxCap;
	GLfloat radius;


public:

	Cylinder(const glm::vec3 & center, GLfloat& height, GLfloat &radius)
	: Object(), center(center), minCap(-height/2), maxCap(height/2), radius(radius)
	{

	}
	virtual ~Cylinder() {}

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* CYLINDER_H */
