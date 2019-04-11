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
	//const float EPSILON = 0.0000001;
	vec3 vertex0 = A;
	vec3 vertex1 = B;
	vec3 vertex2 = C;

	vec3 edge1, edge2, h, s, q;
	GLfloat a,f,u,v;
	edge1 = vertex1 - vertex0;
	edge2 = vertex2 - vertex0;
	h = cross(r.direction, edge2);
	a = dot(edge1, h);
	if (abs(a) < EPSILON)
		return false;    // This ray is parallel to this triangle.

	f = 1.0/a;
	s = r.origin - vertex0;

	u = f * (dot(s,h));
	if (u < 0.0 || u > 1.0)
		return false;

	q = cross(s, edge1);
	v = f * dot(r.direction, q);
	if (v < 0.0 || u + v > 1.0)
		return false;
	// At this stage we can compute t to find out where the intersection point is on the line.
	GLfloat t = f * dot(edge2, q);;
	if (t < EPSILON) // ray intersection
	{
		return false;
	}

	*dist = t;


	if (point)
		*point = r.origin + t*r.direction;;
	if (normal) {

		if (faceNormals) {
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

}


void Triangle::print() const
{
	cout << "TRIANGLE" << endl;
	Object::print();
}




////////// OLD SLOW TRIANGLE INTERSECTION ////////////////

//	vec3 v0v1 = B - A;
//	vec3 v0v2 = C - A;
//	vec3 pvec = cross(r.direction, v0v2);
//	GLfloat det = dot(v0v1, pvec);
//
//	// ray and triangle are parallel if det is close to 0
//	if (fabs(det) < EPSILON) return false;
//
//	GLfloat invDet = 1 / det;
//
//	vec3 tvec = r.origin - A;
//	vec2 uv;
//	uv.x = invDet * dot(tvec, pvec);
//	if (uv.x < 0 || uv.x > 1) return false;
//
//	vec3 qvec = cross(tvec, v0v1);
//
//	uv.y = invDet * dot(r.direction, qvec);
//	if (uv.y < 0 || uv.x + uv.y > 1) return false;
//
//	GLfloat t = invDet * dot(v0v2, qvec);
//
//	if (t <= 0) {
//
//		return false;
//	}
//
//	*dist = t;
//
//	if (point)
//		*point = r.origin + t*r.direction;;
//	if (normal) {
//
//		if (faceNormals) {
//			*normal = this->N;
//		}
//		else {
//			*normal = (1-uv.x-uv.y*AN) + (uv.x*BN) + (uv.y*CN);
//		}
//	}
//
//	vec2 uv2;
//	uv = (1-uv.x-uv.y*Auv) + (uv.x*Buv) + (uv.y*Cuv);
//	if (texColors) {
//		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv2);
//		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv2);
//		texColors->_specularTexColor = this->getSpecularTextureColor(uv2);
//	}
//	if (texCoords) {
//		*texCoords = uv2;
//	}
//	if (properties) {
//		*properties = _properties;
//	}
//
//	++rayTracerStats.numOfHits;
//
//	return true;
//
////	return (t > 0) ? true : false;





//
//
//	// Another close computation - to check validity of the other
//	GLfloat a_dot_n;
//	GLfloat o_dot_n;
//	GLfloat d_dot_n;
//	GLfloat t = 0;
//	vec3    P; // Intersection point
//
//	vec3 APn, BPn, CPn;
//	GLfloat APw, BPw, CPw;
//	GLfloat alpha, beta, gamma;
//
//	++rayTracerStats.numOfIntersectTests;
//
//	// First - find intersection point
//	d_dot_n = dot(r.direction, N);
//	if (d_dot_n - EPSILON == 0 ) {
//		// No intersection, or very close to no intersection
//		return false;
//	}
//
//	a_dot_n = dot(A, N);
//	o_dot_n = dot(r.origin, N);
//	t = (a_dot_n - o_dot_n) / d_dot_n;
//
//	if (t < EPSILON) {
//		// if t < 0, then the triangle is behind the ray, thus no intersection
//		return false;
//	}
//
//
//	// Now check if intersection point given by: o + td is inside the triangle.
//	// We do this using barycentric coordinates
//	// That is that:
//	// 		(P-A) = b(B-A)+y(C-A). WHere P is the intersection point,
//	//		 A, B, C are this triangles vertices, and a,b,y are alpha beta and gamma from barycentric coordinates.
//	P = r.origin + t*r.direction;
//
//	APn = cross(N, C-B) / (dot(cross(N, C-B), A-C));
//	APw = dot(-APn, C);
//	BPn = cross(N, A-C) / (dot(cross(N, A-C), B-A));
//	BPw = dot(-BPn, A);
//	CPn = cross(N, B-A) / (dot(cross(N, B-A), C-B));
//	CPw = dot(-CPn, B);
//
//	alpha = dot(APn, P) + APw;
//	beta  = dot(BPn, P) + BPw;
//	gamma = dot(CPn, P) + CPw;
//
//
//
//	if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) {
//
//		*dist = t;
//		if (point)
//			*point = P;
//		if (normal) {
//
//			if (faceNormals) {
//				*normal = this->N;
//			}
//			else {
//				*normal = (alpha*AN) + (beta*BN) + (gamma*CN);
//			}
//		}
//
//		vec2 uv;
//		uv = (alpha*Auv) + (beta*Buv) + (gamma*Cuv);
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
//	//
	//
	//	// Another close computation - to check validity of the other
	//	GLfloat a_dot_n;
	//	GLfloat o_dot_n;
	//	GLfloat d_dot_n;
	//	GLfloat t = 0;
	//	vec3    P; // Intersection point
	//
	//	vec3 APn, BPn, CPn;
	//	GLfloat APw, BPw, CPw;
	//	GLfloat alpha, beta, gamma;
	//
	//	++rayTracerStats.numOfIntersectTests;
	//
	//	// First - find intersection point
	//	d_dot_n = dot(r.direction, N);
	//	if (d_dot_n - EPSILON == 0 ) {
	//		// No intersection, or very close to no intersection
	//		return false;
	//	}
	//
	//	a_dot_n = dot(A, N);
	//	o_dot_n = dot(r.origin, N);
	//	t = (a_dot_n - o_dot_n) / d_dot_n;
	//
	//	if (t < EPSILON) {
	//		// if t < 0, then the triangle is behind the ray, thus no intersection
	//		return false;
	//	}
	//
	//
	//	// Now check if intersection point given by: o + td is inside the triangle.
	//	// We do this using barycentric coordinates
	//	// That is that:
	//	// 		(P-A) = b(B-A)+y(C-A). WHere P is the intersection point,
	//	//		 A, B, C are this triangles vertices, and a,b,y are alpha beta and gamma from barycentric coordinates.
	//	P = r.origin + t*r.direction;
	//
	//	APn = cross(N, C-B) / (dot(cross(N, C-B), A-C));
	//	APw = dot(-APn, C);
	//	BPn = cross(N, A-C) / (dot(cross(N, A-C), B-A));
	//	BPw = dot(-BPn, A);
	//	CPn = cross(N, B-A) / (dot(cross(N, B-A), C-B));
	//	CPw = dot(-CPn, B);
	//
	//	alpha = dot(APn, P) + APw;
	//	beta  = dot(BPn, P) + BPw;
	//	gamma = dot(CPn, P) + CPw;
	//
	//
	//
	//	if (alpha >= 0 && alpha <= 1 && beta >= 0 && beta <= 1 && gamma >= 0 && gamma <= 1) {
	//
	//		*dist = t;
	//		if (point)
	//			*point = P;
	//		if (normal) {
	//
	//			if (faceNormals) {
	//				*normal = this->N;
	//			}
	//			else {
	//				*normal = (alpha*AN) + (beta*BN) + (gamma*CN);
	//			}
	//		}
	//
	//		vec2 uv;
	//		uv = (alpha*Auv) + (beta*Buv) + (gamma*Cuv);
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
	//		return true;
	//	}
	//
	//	return false;	}
//
//		++rayTracerStats.numOfHits;
//		return true;
//	}
//
//	return false;


















////const float EPSILON = 0.0000001;
//	vec3 vertex0 = A;
//	vec3 vertex1 = B;
//	vec3 vertex2 = C;
//
//	vec3 edge1, edge2, h, s, q;
//	GLfloat a,f,u,v;
//	edge1 = vertex1 - vertex0;
//	edge2 = vertex2 - vertex0;
//	h = cross(r.direction, edge2);
//	a = dot(edge1, h);
//	if (abs(a) < EPSILON)
//		return false;    // This ray is parallel to this triangle.
//
//	f = 1.0/a;
//	s = r.origin - vertex0;
//
//	u = f * (dot(s,h));
//	if (u < 0.0 || u > 1.0)
//		return false;
//
//	q = cross(s, edge1);
//	v = f * dot(r.direction, q);
//	if (v < 0.0 || u + v > 1.0)
//		return false;
//	// At this stage we can compute t to find out where the intersection point is on the line.
//	GLfloat t = f * dot(edge2, q);;
//	if (t < EPSILON) // ray intersection
//	{
//		return false;
//	}
//
//	*dist = t;
//
//
//	if (point)
//		*point = r.origin + t*r.direction;;
//	if (normal) {
//
//		if (faceNormals) {
//			*normal = this->N;
//		}
//		else {
//			*normal = (1.f-u-v)*AN + u*BN + v*CN;
//		}
//	}
//
//	vec2 uv;
//	uv = (1.f-u-v)*Auv + u*Buv + v*Cuv;
//	if (texColors) {
//		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
//		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
//		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
//	}
//	if (texCoords) {
//		*texCoords = uv;
//	}
//	if (properties) {
//		*properties = _properties;
//	}
//
//	++rayTracerStats.numOfHits;
//
//	return true;


