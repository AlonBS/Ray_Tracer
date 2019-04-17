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

		static bool faceNormal;

		vec3 A,B,C ; 		// This triangle vertices
		vec3 AN,BN,CN ; 	// Normals to the above vertices, respectively.
		vec2 Auv, Buv, Cuv; // Vertices texture uv-mapping
		vec3 N;     		// Face normal to the triangle

		bool __iRay2(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties, vec2* texCoords=nullptr);

public:

	Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc);
	Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc, vec2& auv, vec2& buv, vec2& cuv);
	Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc, vec3& vaNorm, vec3& vbNorm, vec3& vcNorm);
	Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc, vec3& vaNorm, vec3& vbNorm, vec3& vcNorm, vec2& auv, vec2& buv, vec2& cuv);


	// This constructor is used when a triangle is a part of a mesh (and the properties are derived from the mesh and model
	Triangle(vec3& va, vec3& vb, vec3& vc,
			 vec3& vaNorm, vec3& vbNorm, vec3& vcNorm,
			 vec2& auv, vec2& buv, vec2& cuv);


	virtual ~Triangle();

	virtual bool intersectsRay(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	bool intersectsRayM(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, vec2* texCoords);

	virtual void print() const;

	static void setFaceNormal() { faceNormal = true;};
};

#endif /* TRIANGLE_H_ */
