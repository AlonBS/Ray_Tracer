
#include "Plain.h"


using namespace glm;


bool Plain::intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties)
{

	GLfloat t = INFINITY;
	vec3 ip, ip2;
	vec3 n;
	Ray tr = this->invTransform() * r; // Transformed ray

	++rayTracerStats.numOfIntersectTests;

	GLfloat nDotD = glm::dot(N, tr.direction);
	if (abs(nDotD) < EPSILON) { /* Ray and plain are parallel - no intersection */
		return false;
	}

	t = glm::dot(N, P-tr.origin) / nDotD;
	if (t < EPSILON) { /* Intersection is behind the eye point */
		return false;
	}

	ip  = tr.origin + t*tr.direction;
	n = N;

	// normals transformation
	n = normalize(this->invTransposeTrans() * n);

	// M * p - to transform point back
	ip2 = ip;
	ip = vec3(this->transform() * vec4(ip, 1.0f));
	// The distance is the length of the original intersection point with the origin of the non transformed ray.
	*dist = length(ip - r.origin);

	if (point) {
		*point = ip;
	}

	if (normal) {
		*normal = n;
	}

	if (texColors) {

		vec2 uv = _textureAt(ip2);

		if (!_objectGlobalProperties.no_bump_maps) {
			if (hasNormalsMap()) {
				*normal = normalize(2.f*this->getNormalFromMap(uv) - 1.0f); // Note we don't need to apply normals transformation here
			}
		}

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);
	}


	if (properties) {
		*properties = this->properties();
	}

	++rayTracerStats.numOfHits;
	return true;

}


vec2 Plain::_textureAt(const vec3& point)
{
	vec2 uv {};

	GLint64 xInt = abs((GLint64)point.x);
	GLfloat xFract = abs(point.x) - xInt;

	GLint64 yInt = abs((GLint64)point.z);
	GLfloat yFract = abs(point.z) - yInt;


	switch (texturePattern) {

	case REPEAT:
		uv.x = xFract;
		if (point.x < 0) {
			uv.x = 1 - xFract;
		}

		uv.y = yFract;
		if (point.z < 0) {
			uv.y = 1 - yFract;
		}

		break;

	case MIRRORED_REPEAT:
		uv.x = xInt % 2 ? xFract : 1 - xFract;
		uv.y = yInt % 2 ? yFract : 1 - yFract;
		break;

	case CLAMP_TO_EDGE:
		uv.x = clamp(point.x, 0.0f, 1.0f);
		uv.y = clamp(point.z, 0.0f, 1.0f);
		break;
	}

	return uv;
}


void Plain::print() const
{
	std::cout << "Point: ("<< P.x << "," << P.y << "," << P.z << ")" << endl;
	std::cout << "Normal: ("<< N.x << "," << N.y << "," << N.z << ")" << endl;
	Object::print();
}



