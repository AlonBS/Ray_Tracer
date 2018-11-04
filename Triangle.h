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


class Triangle: public Object {

private:

		vec3 A,B,C ; 		// This triangle vertices
		vec2 Auv, Buv, Cuv; // Vertices texture uv-mapping
		vec3 AN,BN,CN ; 	// Normals to the above vertices, respectively.
		vec3 N;     		// Face normal to the triangle

		bool __iRay (Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties, vec2* texCoords=nullptr);
		bool __iRay2(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties, vec2* texCoords=nullptr);

public:

	Triangle(vec3& va, vec3& vb, vec3& vc);
	Triangle(vec3& va, vec3& vb, vec3& vc, vec2& auv, vec2& buv, vec2& cuv);
	Triangle(vec3& va, vec3& vb, vec3& vc, vec3& vaNorm, vec3& vbNorm, vec3& vcNorm);
	Triangle(vec3& va, vec3& vb, vec3& vc,
			 vec3& vaNorm, vec3& vbNorm, vec3& vcNorm,
			 vec2& auv, vec2& buv, vec2& cuv);


	virtual ~Triangle();

	virtual bool intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	bool intersectsRayM(Ray &r, GLfloat &dist, vec3* point, vec3* normal, vec2* texCoords);

	virtual const void print() const;
};

#endif /* TRIANGLE_H_ */
