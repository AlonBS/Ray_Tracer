/*
 * Triangle.cpp
 *
 *  Created on: Jan 26, 2018
 *      Author: alonbs
 */

#include "Triangle.h"


Triangle::Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc)
: Object(), A(va), B(vb), C(vc)
{
	Object::_properties = op;
	N = normalize(cross(C-B,A-B)); // Compute the face normal
	AN = vec3(0.0f, 0.0f, 0.0f);
	BN = vec3(0.0f, 0.0f, 0.0f);
	CN = vec3(0.0f, 0.0f, 0.0f);
	Auv = vec2(0.0f, 0.0f);
	Buv = vec2(0.0f, 0.0f);
	Cuv = vec2(0.0f, 0.0f);
}


Triangle::Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc, vec2& auv, vec2& buv, vec2& cuv)
: Object(), A(va), B(vb), C(vc), Auv(auv), Buv(buv), Cuv(cuv)
{
	Object::_properties = op;
	N = normalize(cross(C-B,A-B)); // Compute the face normal
	AN = vec3(0.0f, 0.0f, 0.0f);
	BN = vec3(0.0f, 0.0f, 0.0f);
	CN = vec3(0.0f, 0.0f, 0.0f);

}



Triangle::Triangle(ObjectProperties& op, vec3& va, vec3& vb, vec3& vc, vec3& vaNorm, vec3& vbNorm, vec3& vcNorm)
: Object(), A(va), B(vb), C(vc), AN(vaNorm), BN(vbNorm), CN(vcNorm)
{
	Object::_properties = op;
	N = normalize(cross(C-B,A-B)); // Compute the face normal

}


Triangle::Triangle(ObjectProperties& op,
				   vec3& va, vec3& vb, vec3& vc,
			 	   vec3& vaNorm, vec3& vbNorm, vec3& vcNorm,
				   vec2& auv, vec2& buv, vec2& cuv)
: Object(), A(va), B(vb), C(vc),
  	  	    AN(vaNorm), BN(vbNorm), CN(vcNorm),
  	  	  	Auv(auv), Buv(buv), Cuv(cuv)
{
	Object::_properties = op;
	N = normalize(cross(C-B,A-B)); // Compute the face normal
}





Triangle::Triangle(vec3& va, vec3& vb, vec3& vc,
			 	   vec3& vaNorm, vec3& vbNorm, vec3& vcNorm,
				   vec2& auv, vec2& buv, vec2& cuv)
: Object(), A(va), B(vb), C(vc),
  	  	    AN(vaNorm), BN(vbNorm), CN(vcNorm),
  	  	  	Auv(auv), Buv(buv), Cuv(cuv)
{
	N = normalize(cross(C-B,A-B)); // Compute the face normal
}


Triangle::~Triangle()
{
}


bool Triangle::intersectsRay(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{
	return __iRay2(r, dist, point, normal, texColors, properties);
}



bool
Triangle::intersectsRayM(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, vec2* texCoords)
{
	return __iRay2(r, dist, point, normal, nullptr, nullptr, texCoords);
//	res2 = __iRay2(r, &dist2, &point2, &norm2, nullptr, nullptr, &texCoords2);
}


bool
Triangle::__iRay2(const Ray& r,
				  GLfloat* dist,
				  vec3* point,
				  vec3* normal,
				  ObjectTexColors* texColors,
				  ObjectProperties* properties,
				  vec2* texCoords)
{
	vec3 edge1, edge2, tvec, pvec, qvec;
	GLfloat det, inv_det;
	GLfloat u, v, t;

	edge1 = B-A;
	edge2 = C-A;

	pvec = cross(r.direction, edge2);
	det = dot(edge1, pvec);

	if (det > EPSILON) {

		tvec = r.origin - A;
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
		tvec = r.origin - A;

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
			*normal = (1.f-u-v)*AN + u*BN + v*CN;
		}
	}

	vec2 uv;
	uv = (1.f-u-v)*Auv + u*Buv + v*Cuv;
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

	++rayTracerStats.numOfHits;

	return true;


	// PAPER VERSION


//	//const float EPSILON = 0.0000001;
//		vec3 vertex0 = A;
//		vec3 vertex1 = B;
//		vec3 vertex2 = C;
//
//		vec3 edge1, edge2, h, s, q;
//		GLfloat a,f,u,v;
//		edge1 = vertex1 - vertex0;
//		edge2 = vertex2 - vertex0;
//		h = cross(r.direction, edge2);
//		a = dot(edge1, h);
//		if (abs(a) < EPSILON)
//			return false;    // This ray is parallel to this triangle.
//
//		f = 1.0/a;
//		s = r.origin - vertex0;
//
//		u = f * (dot(s,h));
//		if (u < 0.0 || u > 1.0)
//			return false;
//
//		q = cross(s, edge1);
//		v = f * dot(r.direction, q);
//		if (v < 0.0 || u + v > 1.0)
//			return false;
//		// At this stage we can compute t to find out where the intersection point is on the line.
//		GLfloat t = f * dot(edge2, q);;
//		if (t < EPSILON) // ray intersection
//		{
//			return false;
//		}
//
//		*dist = t;
//
//
//		if (point)
//			*point = r.origin + t*r.direction;;
//		if (normal) {
//
//			if (faceNormals) {
//				*normal = this->N;
//			}
//			else {
//				*normal = (1.f-u-v)*AN + u*BN + v*CN;
//			}
//		}
//
//		vec2 uv;
//		uv = (1.f-u-v)*Auv + u*Buv + v*Cuv;
//		if (texColors) {
//			texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
//			texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
//			texColors->_specularTexColor = this->getSpecularTextureColor(uv);
//		}
//		if (texCoords) {
//			*texCoords = uv;
//		}
//		if (properties) {
//			*properties = _properties;
//		}
//
//		++rayTracerStats.numOfHits;
//
//		return true;
//


}


void Triangle::print() const
{
	cout << "TRIANGLE" << endl;
	Object::print();
}
