/*
 * Cylinder.h
 *
 *  Created on: Feb 22, 2018
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

	vec3 _normalAt(const vec3& point, bool minCapIntersect, bool maxCapIntersect);



public:

	Cylinder(const ObjectProperties& properties, const ObjectTransforms& transforms,
			 const glm::vec3 & center, GLfloat& height, GLfloat &radius)
	: Object(properties, transforms),
	  center(center), minCap(-height/2.0f), maxCap(height/2.0f), radius(radius)
	{
	}
	virtual ~Cylinder() {}

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;

	virtual void computeBoundingBox();

};

#endif /* CYLINDER_H */
