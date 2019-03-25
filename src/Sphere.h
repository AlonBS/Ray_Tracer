/*
 * Sphere.h
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#ifndef SPHERE_H_
#define SPHERE_H_

#include <GL/glew.h>
#include <GL/glut.h>



#include "Object.h"


class Sphere: public Object {

private:

	glm::vec3 center;
	GLfloat radius;


public:

	Sphere(const glm::vec3 & center, GLfloat &radius)
	: Object(), center(center), radius(radius)
	{
		vec3 minBound = vec3(center.x-radius, center.y-radius, center.z-radius);
		vec3 maxBound = vec3(center.x+radius, center.y+radius, center.z+radius);
		this->bbox = new BoundingBox(minBound, maxBound);
	}
	virtual ~Sphere() {	}

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* SPHERE_H_ */
