/*
 * Object.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: alonbs
 */

#include "Object.h"

using namespace glm;

Object::GlobalObjectProperties Object::_objectGlobalProperties =
{
	.face_normals = false,
	.no_bump_maps = false
};


Object::Object(const ObjectProperties& properties, const ObjectTransforms& transforms)
: _properties(properties), _transforms(transforms)
{
	_ambientTexture = nullptr;
	_diffuseTexture = nullptr;
	_speularTexture = nullptr;
	_normalMap = nullptr;

	bbox = nullptr;
}



Object::Object()
{
	_properties = {};
	_transforms = {};

	_ambientTexture = nullptr;
	_diffuseTexture = nullptr;
	_speularTexture = nullptr;
	_normalMap = nullptr;

	bbox = nullptr;
}



Object::~Object() {}


void Object::setTextures(shared_ptr<const Image>& texture, shared_ptr<const Image>* normalMap)
{
	if (texture == nullptr)
		return;

	// TODO - Currently, an object has one texture. Its meshes can have different types of texture.
	// The texture bound from .rt file is global, and is similar in all attributes. Next phases, different global textures will be supported.
	// This idiom will enable us to quickly expand the functionality.
	this->_ambientTexture = texture;
	this->_diffuseTexture = texture;
	this->_speularTexture = texture;

	if (normalMap)
		this->_normalMap = *normalMap;

}


std::ostream& operator<< (std::ostream& out, const Object & obj)
{
	obj.print();
	return out;
}


void Object::print() const
{
	printVec3("Ambient", _properties._ambient);
	printVec3("_emission", _properties._emission);
	printVec3("_diffuse", _properties._diffuse);
	printVec3("_specular", _properties._specular);
	cout << "Shininess: " << _properties._shininess << endl;
}


vec3 Object::getAmbientTextureColor(const vec2& uv) const
{
	return getTextureColor(this->_ambientTexture.get(), uv);
}

vec3 Object::getDiffuseTextureColor(const vec2& uv) const
{
	return getTextureColor(this->_diffuseTexture.get(), uv);
}

vec3 Object::getSpecularTextureColor(const vec2& uv) const
{
	return getTextureColor(this->_speularTexture.get(), uv);
}


vec3 Object::getNormalFromMap(const vec2& uv) const
{
	// Note that the caller must be aware that if no normal map is bound, irrelevant value is returned
	return getTextureColor(this->_normalMap.get(), uv);
}


bool Object::bBoxIntersectsRay(const Ray& r, GLfloat* t_near) const
{
	if (!bbox) {
		*t_near = -INFINITY;
		return true;
	}

	return bbox.get()->intersectsRay(r, t_near);
}

bool Object::bBoxIntersectsRay(const Ray& tr, GLfloat* t_near)
{
	return static_cast<const Object &>(*this).bBoxIntersectsRay(tr, t_near);
}


vec3 Object::__GetTBNAndNorm(vec3 N, vec3 T, const vec2& uv) const
{

	N = this->_transforms._invTransposeTrans * N;
	T = this->_transforms._invTransposeTrans * T;
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N,T));
	mat3 TBN = mat3(T,B,N);

	vec3 norm = normalize(2.f*getNormalFromMap(uv) - 1.0f);
	return normalize(TBN * norm);
}




//vec3 Object::__GetTBNAndNorm(vec3& N, vec3& T, const vec2& uv) const
//{
//
//	T = normalize(this->_transforms._invTransposeTrans * cross(vec3(0,1,0), N));
//	N = this->_transforms._invTransposeTrans * N;
//	T = normalize(T - dot(T, N) * N);
//	vec3 B = normalize(cross(N,T));
//	mat3 TBN = mat3(T,B,N);
//
//	vec3 norm = normalize(2.f*getNormalFromMap(uv) - 1.0f);
//	return normalize(TBN * norm);
//}
//


