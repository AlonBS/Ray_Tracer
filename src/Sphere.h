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

	Sphere(const ObjectProperties& properties, const ObjectTransforms& transforms,
		   const glm::vec3 & center, GLfloat &radius)
	: Object(properties, transforms),
	  center(center), radius(radius)
	{
	}
	virtual ~Sphere() {	}

	virtual bool intersectsRay(
			const Ray &r,
			GLfloat* dist,
			vec3* point,
			vec3* normal,
			ObjectTexColors* texColors,
			ObjectProperties* properties) const;

	virtual void print() const;

	virtual void computeBoundingBox();

};



#endif /* SPHERE_H_ */
