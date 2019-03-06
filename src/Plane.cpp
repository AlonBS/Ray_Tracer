
#include "Plane.h"


using namespace glm;


bool Plane::intersectsRay(Ray &r, GLfloat &dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{

	Ray tr = this->invTransform() * r; // Transformed ray

	GLfloat t = INFINITY;
	vec3 ip, ip2;
	vec3 n;

	GLfloat nDotD = glm::dot(N, tr.direction);
	if (abs(nDotD) < 0.001f) { /* Ray and plane are parallel - no intersection */
		return false;
	}

	t = glm::dot(N, P-tr.origin) / nDotD;
	if (t < 0) { /* Intersection is behind the eye point */
		return false;
	}

	ip  = tr.origin + t*tr.direction;
	n = N;
	// Normal transformation
	n = normalize(vec3(mat3(this->invTransposeTrans()) * n));

	// M * p - to transform point back
	ip2 = ip;
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}
	if (normal) {
		*normal = n;
	}
	if (texColors) {


		// Mirror Textures
		vec2 uv;
		GLint64 xInt = abs((GLint64)ip2.x);
		GLfloat xFract = abs(ip2.x) - xInt;
		uv.x = xInt % 2 ? xFract : 1 - xFract;

		GLint64 yInt = abs((GLint64)ip2.z);
		GLfloat yFract = abs(ip2.z) - yInt;
		uv.y = yInt % 2 ? yFract : 1 - yFract;

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}
	if (properties) {
		*properties = this->properties();
	}

	return true;

}

//
//
//vec2 Box::_textureAt(const vec3& point)
//{
//	vec2 uv {};
//
//	if (abs(point.x - bounds[0].x) < EPSILON ||
//	    abs(point.x - bounds[1].x) < EPSILON) { // on YZ plane
//
//		uv.x = (point.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
//		uv.y = (point.z - bounds[0].z) / (bounds[1].z - bounds[0].z);
//
//	}
//	else if (abs(point.y - bounds[0].y) < EPSILON ||
//			 abs(point.y - bounds[1].y) < EPSILON) { // on XZ plane
//
//		uv.x = (point.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
//		uv.y = (point.z - bounds[0].z) / (bounds[1].z - bounds[0].z);
//	}
//
//	else { // on XY plane
//
//		uv.x = (point.x - bounds[0].x) / (bounds[1].x - bounds[0].x);
//		uv.y = (point.y - bounds[0].y) / (bounds[1].y - bounds[0].y);
//	}
//
//	// Again - we don't care too much about edges
//	return uv;
//}


void Plane::print() const
{
	std::cout << "Point: ("<< P.x << "," << P.y << "," << P.z << ")" << endl;
	std::cout << "Normal: ("<< N.x << "," << N.y << "," << N.z << ")" << endl;
	Object::print();
}


