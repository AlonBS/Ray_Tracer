/*
 * Box.h
 *
 *  Created on: Feb 25, 2019
 *      Author: alonbs
 */

#ifndef BOX_H
#define BOX_H

#include <GL/glew.h>
#include <GL/glut.h>


#include "Object.h"


class Box: public Object {

private:

	vec3 _normalAt(const vec3 &point);
	vec2 _textureAt(const vec3& point);

	vec3 bounds[2]; // Yes! public

public:


	Box()
	: Object()
	{

	}
	Box(vec3 & minBound, vec3& maxBound)
	: Object()
	{
		bounds[0] = {minBound};
		bounds[1] = {maxBound};
	}
	virtual ~Box() {}

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* BOX_H */
