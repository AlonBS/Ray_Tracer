/*
 * Box.h
 *
 *  Created on: Mar 10, 2019
 *      Author: alonbs
 */

#ifndef BOUNDING_VOLUME_H
#define BOUNDING_VOLUME_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <vector>

#include "Object.h"

typedef struct SlabDist {

	GLfloat dNear;
	GLfloat dFar;

}SlabDist;


/**
 * This is an implementation of the BV structure as suggested by Kay Kajiya (1986)
 */
class BoundingVolume {

private:

	static const int NUM_OF_SET_NORMALS = 7;
	static const vec3 planeSetNormals[NUM_OF_SET_NORMALS];

	SlabDist dists[7];


public:

	BoundingVolume();

	void computeBounds(vector<Vertex>& vertices);


	void print() const;

};

#endif /* BOUNDING_VOLUME_H */
