/*
 * Triangle.cpp
 *
 *  Created on: Jan 26, 2018
 *      Author: alonbs
 */

#include "Triangle.h"

#include <glm/detail/type_vec.hpp>

Triangle::Triangle(Vertex& va, Vertex& vb, Vertex& vc)
: Object(), A(std::move(va)), B(std::move(vb)), C(std::move(vc))
{
	N = normalize(cross(C.Position-B.Position,A.Position-B.Position)); // Compute the face normal
	TBN_A = __calcTBNMat(A);
	TBN_B = __calcTBNMat(B);
	TBN_C = __calcTBNMat(C);
}


Triangle::Triangle(ObjectProperties& op, Vertex& va, Vertex& vb, Vertex& vc)
: Object(), A((std::move(va))), B((std::move(vb))), C((std::move(vc)))
{
	Object::_properties = op;
	N = normalize(cross(C.Position-B.Position,A.Position-B.Position)); // Compute the face normal
	TBN_A = __calcTBNMat(A);
	TBN_B = __calcTBNMat(B);
	TBN_C = __calcTBNMat(C);

}


Triangle::~Triangle()
{
}


bool Triangle::intersectsRay(
		const Ray& r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties) const
{
	return __iRay2(r, dist, point, normal, texColors, properties);
}



bool
Triangle::intersectsRayM(
		const Ray& r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		vec2* texCoords,
		mat3* TBN) const
{
	return __iRay2(r, dist, point, normal, nullptr, nullptr, texCoords, TBN);
}


bool
Triangle::__iRay2(
		const Ray& r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties,
		vec2* texCoords,
		mat3* TBN) const
{
	vec3 edge1, edge2, tvec, pvec, qvec;
	GLfloat det, inv_det;
	GLfloat u, v, t;

	updateStats(INCREMENT_INTERSECTION_TESTS_COUNT);

	edge1 = B.Position-A.Position;
	edge2 = C.Position-A.Position;

	pvec = cross(r.direction, edge2);
	det = dot(edge1, pvec);

	if (det > EPSILON) {

		tvec = r.origin - A.Position;
		u = dot(tvec, pvec);
		if (u < 0.0f || u > det)
			return false;

		qvec = cross(tvec, edge1);

		v = dot(r.direction, qvec);
		if (v < 0.0f || u + v > det)
			return false;
	}

	else if (det < -EPSILON)
	{
		tvec = r.origin - A.Position;

		u = dot(tvec, pvec);
		if (u > 0.0f || u < det)
			return false;

		qvec = cross(tvec, edge1);

		v = dot(r.direction, qvec);
		if (v > 0.0f || u + v < det)
			return false;
	}
	else {
		return false;
	}

	inv_det = 1.0f / det;

	t = inv_det * dot(edge2, qvec);

	if (t < EPSILON) // ray intersection
	{
		return false;
	}

	u *= inv_det;
	v *= inv_det;

	*dist = t;
	if (point)
		*point = r.origin + t*r.direction;;
	if (normal) {

		if (_objectGlobalProperties.face_normals) {
			*normal = this->N;
		}
		else {
			*normal = (1.f-u-v)*A.Normal + u*B.Normal + v*C.Normal;
		}
	}

	vec2 uv;
	uv = (1.f-u-v)*A.TexCoords + u*B.TexCoords + v*C.TexCoords;
	if (texColors) {
		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);

	}
	if (texCoords) {
		*texCoords = uv;
	}
	if (properties) {
		*properties = _properties;
	}

	if (TBN) {

		*TBN = (1.f-u-v)*TBN_A + u*TBN_B + v*TBN_C;
	}

	updateStats(INCREMENT_HITS_COUNT);

	return true;

}


mat3 Triangle::__calcTBNMat(const Vertex& v) const
{
	vec3 T = normalize(v.Tangent);
	vec3 N = normalize(v.Normal);
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	// then retrieve perpendicular vector B with the cross product of T and N
	vec3 B = cross(N, T);
	return mat3(T, B, N);
}


void Triangle::print() const
{
	cout << "TRIANGLE" << endl;
	Object::print();
}
