/*
 * Plane.h
 *
 *  Created on: Mar 4, 2019
 *      Author: alonbs
 */

#ifndef PLANE_H
#define PLANE_H

#include <GL/glew.h>
#include <GL/glut.h>


#include "Object.h"

/**
 * The plane is represented using a point P on the plane, and a Normal N to the plane.
 */
class Plane: public Object {

private:


	vec3 P;
	vec3 N;

	vec3 U,V;


	vec3 _normalAt(const vec3 &point);
	vec2 _textureAt(const vec3& point);

	void _computePrimaryTexDir();


public:

	Plane(vec3& point, vec3& normal)
	: Object(), P(point), N(normal)
	{
		_computePrimaryTexDir();
	}

	virtual ~Plane() {}

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* PLANE_H */
