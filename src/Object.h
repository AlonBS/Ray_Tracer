/*
 * Object.h
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <GL/glew.h>

#include "AABB.h"
#include "Ray.h"
#include "General.h"
#include "Image.h"

using namespace glm;


class Object {

private:


	Image *_ambientTexture;
	Image *_diffuseTexture;
	Image *_speularTexture;



protected:


	ObjectProperties _properties;
	ObjectTransforms _transforms;

	vec3 getTextureColor(Image *texture, vec2& uv);

	AABB* bbox;



public:


	Object();
	Object(const ObjectProperties& properties, const ObjectTransforms& transforms);
	virtual ~Object();

	void setTexture(Image *texture);
	friend std::ostream& operator<< (std::ostream& out, const Object & obj);
	virtual void print() const;

	virtual vec3 getAmbientTextureColor(vec2& uv);
	virtual vec3 getDiffuseTextureColor(vec2& uv);
	virtual vec3 getSpecularTextureColor(vec2& uv);


	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) = 0;
	bool bBoxIntersectsRay(const Ray& tr, GLfloat* t_near);
	virtual void computeBoundingBox() { this->bbox = nullptr; }



	auto properties() -> ObjectProperties& { return _properties; };
	auto transform() -> mat4& { return _transforms._transform; }
	auto invTransform() -> mat4& { return _transforms._invTransform; }
	auto invTransposeTrans() -> mat3& { return _transforms._invTransposeTrans; }

};




#endif /* OBJECT_H_ */
