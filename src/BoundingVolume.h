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
//#include "Model.h"
#include "Mesh.h"

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

	/* Only for models - i.e. for complex objects */
	struct Extent {

		Extent(Mesh* mesh);
		void __computeBounds(vector<Vertex>& vertices);
		bool intersectRay(const Ray &r, GLfloat* t_near);
		void print() const;


		Mesh* mesh;
		SlabDist dists[NUM_OF_SET_NORMALS];
	};

	Object* boundObject; /* Currently -we only support one object per bounding volume */
	vector<Extent*> extents;

public:



	BoundingVolume(Object* obj);
	~BoundingVolume();

	//bool intersectRay(IN const Ray& r, IN const GLfloat& minDist, OUT Intersection* HIT);


	bool intersectRay(const Ray &r,
					  GLfloat &minDist,
					  GLfloat* dist,
					  vec3* point,
					  vec3* normal,
					  ObjectTexColors* texColors,
					  ObjectProperties* properties);



	void print() const;

};

#endif /* BOUNDING_VOLUME_H */
