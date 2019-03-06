
#include "Plane.h"


using namespace glm;


void Plane::_computePrimaryTexDir()
{
    vec3 a = cross(N, vec3(1, 0, 0));
    vec3 b = cross(N, vec3(0, 1, 0));
    vec3 c = cross(N, vec3(0, 0, 1));

    printVec3("A", a);
    printVec3("B", b);
    printVec3("C", c);

    vec3 max_ab = length(a) > length(b) ? a : b;
    vec3 maxDir = length(max_ab) > length(c) ? max_ab : c;

    printVec3("max_ab", max_ab);
    printVec3("maxDir", maxDir);
    U = normalize(maxDir);
    V = cross(N, maxDir);

    printVec3("U", U);
    printVec3("V", V);
}


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

//		[0-1]
//
//		[2]
//		[0-2][2-4][4-6]]

		printVec3("IP", ip);
//		GLfloat dd = length(ip);

		vec2 uv;
		GLfloat intPart;
//		uv.x = modf (ip.x , &intPart);
		uv.x = abs(ip.x - (long)ip.x);
//		uv.y = modf (ip.z, &intPart);
		uv.y = abs(ip.z -(long)ip.z);

//		uv.x = dot(U, ip);
//		uv.y = dot(V, ip);
//		uv.x = dd*U;
//		uv.x = (ip.x) / 640;
//		uv.y = (ip.z-480) / 480;
		printVec2("UV", uv);
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



