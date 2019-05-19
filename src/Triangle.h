/*
 * Triangle.h
 *
 *  Created on: Jan 26, 2018
 *      Author: alonbs
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "Object.h"

#include <glm/glm.hpp>
#include <vector>


class Triangle: public Object {

private:

		Vertex A, B, C;
		vec3 N;
		mat3 TBN_A, TBN_B, TBN_C;

		bool __iRay2(
				const Ray& r,
				GLfloat* dist,
				vec3* point,
				vec3* normal,
				ObjectTexColors* texColors,
				ObjectProperties* properties,
				vec2* texCoords=nullptr,
				mat3* TBN=nullptr) const;

		mat3 __calcTBNMat(const Vertex& v) const;

public:

	Triangle(Vertex& va, Vertex& vb, Vertex& vc); 	// This constructor is used when a triangle is a part of a mesh (and the properties are derived from the mesh and model
	Triangle(ObjectProperties& op, Vertex& va, Vertex& vb, Vertex& vc);

	vector<vec3> getVerticesPos() const { return vector<vec3>{A.Position, B.Position, C.Position}; }


	virtual ~Triangle();

	virtual bool intersectsRay(
			const Ray& r,
			GLfloat* dist,
			vec3* point,
			vec3* normal,
			ObjectTexColors* texColors,
			ObjectProperties* properties) const;


	bool intersectsRayM(
			const Ray& r,
			GLfloat* dist,
			vec3* point,
			vec3* normal,
			vec2* texCoords,
			mat3* TBN) const;

	virtual void print() const;

};

#endif /* TRIANGLE_H_ */
