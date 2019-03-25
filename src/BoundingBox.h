/*
 * Box.h
 *
 *  Created on: Mar 25, 2019
 *      Author: alonbs
 */

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include <GL/glew.h>
#include <GL/glut.h>

#include "Ray.h"
#include <glm/glm.hpp>

using namespace glm;


class BoundingBox {

private:


public:

	vec3 bounds[2]; // Yes! public

	BoundingBox(){ }

	BoundingBox(vec3 & minBound, vec3& maxBound)
	{
		bounds[0] = {minBound};
		bounds[1] = {maxBound};
	}

	vec3 centroid() const { return (bounds[0] + bounds[1]) * 0.5f; }

	bool intersectsRay(const Ray& tr, GLfloat* t_near);

	void print() const;
};

#endif /* BOUNDING_BOX_H */
