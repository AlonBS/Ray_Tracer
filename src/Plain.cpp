
#include "Plain.h"


using namespace glm;


bool Plain::intersectsRay(
		const Ray &r,
		GLfloat* dist,
		vec3* point,
		vec3* normal,
		ObjectTexColors* texColors,
		ObjectProperties* properties) const
{

	GLfloat t = INFINITY;
	vec3 ip, ip2;
	vec3 n;
	Ray tr = this->_transforms._invTransform * r; // Transformed ray

	updateStats(INCREMENT_INTERSECTION_TESTS_COUNT);

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
	n = normalize(this->_transforms._invTransposeTrans * n);

	// M * p - to transform point back
	ip2 = ip;
	ip = vec3(this->_transforms._transform * vec4(ip, 1.0f));
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

		texColors->_ambientTexColor  = this->getAmbientTextureColor(uv);
		texColors->_diffuseTexColor  = this->getDiffuseTextureColor(uv);
		texColors->_specularTexColor = this->getSpecularTextureColor(uv);

		if (!_objectGlobalProperties.no_bump_maps && _normalMap) {
			vec3 T = vec3(0,0,1);
			*normal = __GetTBNAndNorm(N, T, uv); // We need the original normal, not the transformed one.
		}
	}

	if (properties) {
		*properties = this->_properties;
	}

	updateStats(INCREMENT_HITS_COUNT);
	return true;

}


vec2 Plain::_textureAt(const vec3& point) const
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



