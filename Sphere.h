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

	vec3 __calculateTexture(vec3& p);


public:

	Sphere(const glm::vec3 & center, GLfloat &radius)
	: Object(), center(center), radius(radius)
	{

	}
	virtual ~Sphere() {}

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual const void print() const;
};

#endif /* SPHERE_H_ */
