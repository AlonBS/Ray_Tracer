/*
 * Plane.h
 *
 *  Created on: Mar 4, 2019
 *      Author: alonbs
 */

#ifndef PLAIN_H
#define PLAIN_H

#include <GL/glew.h>
#include <GL/glut.h>


#include "Object.h"

enum TexturePattern {

	// Same as open-gl mapping
	REPEAT,
	MIRRORED_REPEAT,
	CLAMP_TO_EDGE,
	/* CLAMP_TO_BORDER  - currently this is not supported */

};

/**
 * The plain is represented using a point P on the plane, and a Normal N to the plain.
 */
class Plain: public Object {

private:


	vec3 P;
	vec3 N;

	enum TexturePattern texturePattern;


	vec2 _textureAt(const vec3& point);


public:

	Plain(const ObjectProperties& properties, const ObjectTransforms& transforms,
		  enum TexturePattern pattern)
	: Object(properties, transforms),
	  P(vec3(0,0,0)), N(vec3(0,1,0)), texturePattern(pattern)
	{
	}

	virtual ~Plain() {}

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);

	virtual void print() const;
};

#endif /* PLAIN_H */
