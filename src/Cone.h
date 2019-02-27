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
	GLfloat radius;


public:

	Cone(const glm::vec3 & center, GLfloat& height, GLfloat &radius)
	: Object(), center(center), minCap(-height/2.0f), maxCap(height/2.0f), radius(radius)
	{

	}
	virtual ~Cone() {}

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* CONE_H */
