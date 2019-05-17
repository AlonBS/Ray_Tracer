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
protected:

	typedef struct GlobalObjectProperties {
		bool face_normals;
		bool no_bump_maps;
	} GlobalObjectProperties ;

	static GlobalObjectProperties _objectGlobalProperties;


	ObjectProperties _properties;
	ObjectTransforms _transforms;
	shared_ptr<const Image> _ambientTexture;
	shared_ptr<const Image> _diffuseTexture;
	shared_ptr<const Image> _speularTexture;
	shared_ptr<const Image> _normalMap;

	unique_ptr<const AABB> bbox;


public:


	Object();
	Object(const ObjectProperties& properties, const ObjectTransforms& transforms);
	virtual ~Object();

	void setTextures(shared_ptr<const Image>& texture, shared_ptr<const Image>* normalsMap=nullptr);
	friend std::ostream& operator<< (std::ostream& out, const Object & obj);
	virtual void print() const;

	virtual vec3 getAmbientTextureColor(const vec2& uv) const;
	virtual vec3 getDiffuseTextureColor(const vec2& uv) const;
	virtual vec3 getSpecularTextureColor(const vec2& uv) const;

	vec3 getNormalFromMap(const vec2& uv) const;


	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const = 0;

	bool bBoxIntersectsRay(const Ray& tr, GLfloat* t_near) const;
	bool bBoxIntersectsRay(const Ray& tr, GLfloat* t_near);
	virtual void computeBoundingBox() { this->bbox = nullptr; }


	static void setFaceNormals() { _objectGlobalProperties.face_normals = true;}
	static void setNoBumpMaps() { _objectGlobalProperties.no_bump_maps = true;}

};






#endif /* OBJECT_H_ */
