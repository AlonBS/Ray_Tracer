/*
 * Object.h
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include "Ray.h"
#include <GL/glew.h>
#include "General.h"
#include "Image.h"

using namespace glm;

typedef struct ObjectProperties {

	vec3 _ambient;
	vec3 _emission;
	vec3 _diffuse;
	vec3 _specular;
	GLfloat _shininess;

}ObjectProperties;


typedef struct ObjectTransforms {

	mat4 _transform;
	mat4 _invTransform;	  	 // We compute it once, instead of each intersection test
	mat3 _invTransposeTrans; // For normals transforms - notice 3x3

}ObjectTransforms;


typedef struct ObjectTexColors {


	vec3 _ambientTexColor;
	vec3 _diffuseTexColor;
	vec3 _specularTexColor;


}ObjectTexColors;


// TODO - CHANGE
typedef struct Intersection {

	bool isValid;

	vec3 point;
	vec3 normal;
	ObjectTexColors texColors;

	ObjectProperties properties; // The object's properties at intersection

}Intersection;


class Object {

private:


	Image *_ambientTexture;
	Image *_diffuseTexture;
	Image *_speularTexture;

protected:


	ObjectProperties _properties;
	ObjectTransforms _transforms;

	vec3 getTextureColor(Image *texture, vec2& uv);

	//bool _textured;

public:

	virtual vec3 getAmbientTextureColor(vec2& uv);
	virtual vec3 getDiffuseTextureColor(vec2& uv);
	virtual vec3 getSpecularTextureColor(vec2& uv);


	Object();
	virtual ~Object();

	void setTexture(Image *texture);

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) = 0;


	friend std::ostream& operator<< (std::ostream& out, const Object & obj);

	virtual void print() const;

	auto properties() -> ObjectProperties& { return _properties; };
	auto ambient () -> vec3& { return _properties._ambient; }
	auto emission() -> vec3& { return _properties._emission; }
	auto diffuse () -> vec3& { return _properties._diffuse; }
	auto specular() -> vec3& { return _properties._specular; }
	auto shininess() -> GLfloat& { return _properties._shininess; }

	auto transform() -> mat4& { return _transforms._transform; }
	auto invTransform() -> mat4& { return _transforms._invTransform; }
	auto invTransposeTrans() -> mat3& { return _transforms._invTransposeTrans; }


};




#endif /* OBJECT_H_ */
