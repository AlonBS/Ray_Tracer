/*
 * General.h
 *
 *  Created on: Feb 10, 2018
 *      Author: alonbs
 */

#ifndef GENERAL_H_
#define GENERAL_H_

#include <cstdint>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <iostream>
#include <future>
#include "Image.h"
#include "Ray.h"



using namespace glm;
using namespace std;

typedef struct ObjectProperties {

	vec3 _ambient;
	vec3 _emission;
	vec3 _diffuse;
	vec3 _specular;
	GLfloat _shininess;

	vec3 _reflection;
	GLfloat _reflectionBlur;
	vec3 _refraction;
	GLfloat _refractionIndex;
	GLfloat _refractionBlur;

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


struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
//    // tangent
    glm::vec3 Tangent;
//    // bitangent
    glm::vec3 Bitangent;
};


// TODO - CHANGE
typedef struct Intersection {

	bool isValid;

	vec3 point;
	vec3 normal;
	ObjectTexColors texColors;

	ObjectProperties properties; // The object's properties at intersection

}Intersection;


typedef struct RayTracerStats {

	atomic<GLuint64> numOfRays;
	atomic<GLuint64> numOfIntersectTests;
	atomic<GLuint64> numOfHits;

}RayTracerStats;

extern RayTracerStats rayTracerStats;
//extern bool faceNormals;

void clearStats();

#define IN
#define OUT


//const GLfloat EPSILON  = 0.00000000001f;
const GLfloat EPSILON  = 0.0001f;
const GLfloat MAX_RECURSION_DEPTH = 5.f;
const GLfloat RECURSION_DEFAULT_DEPTH = 2.f;


//const GLfloat INFINITY = FLT_MAX;
const GLfloat PI = 3.1415926f;
const vec3 COLOR_WHITE = vec3(1.0f, 1.0f, 1.0f);
const vec3 COLOR_BLACK = vec3(0.0f, 0.0f, 0.0f);
const vec3 NO_DIRECTION = COLOR_BLACK;

const vec3 NEGLIGENT_CONTRIBUTION = vec3(0.5f * EPSILON, 0.5f * EPSILON, 0.5f * EPSILON);
const vec3 FULL_CONTRIBUTION = vec3(1.0f, 1.0f, 1.0f);


// Refractions indecies
const GLfloat VOID_INDEX = 1.000f;
const GLfloat AIR_INDEX = 1.0003f;
const GLfloat WATER_INDEX = 1.333f;
const GLfloat GLASS_INDEX = 1.52f;


inline void
printVec2(const string& name, const vec2& vec)
{
	printf("%s: (%04f,%04f)\n", name.c_str(), vec.x, vec.y);
}

inline void
printVec3(const string& name, const vec3& vec)
{
	printf("%s: (%04f,%04f, %04f)\n", name.c_str(), vec.x, vec.y, vec.z);
}


inline void
printMat4(const string& name, const mat4& mat)
{
	cout << name << ":" << endl;
	for (int i = 0; i < 4 ; ++i) {
		for (int j = 0 ; j < 4 ; ++j) {

			cout << "\t" << mat[i][j];
		}
		cout << endl;
	}
}

inline bool
equalToVec2(const vec2& v1, const vec2& v2)
{
	return glm::abs(v1.x - v2.x) < EPSILON &&
		   glm::abs(v1.y - v2.y) < EPSILON;

}

inline bool
equalToVec3(const vec3& v1, const vec3& v2)
{
	return glm::abs(v1.x - v2.x) < EPSILON &&
		   glm::abs(v1.y - v2.y) < EPSILON &&
		   glm::abs(v1.z - v2.z) < EPSILON;
}

inline bool
vec3IsValid(const vec3& v)
{
	return !(isnan(v.x) || isnan(v.y) || isnan(v.z));
}

inline bool rayIsValid(const Ray& ray)
{
	return vec3IsValid(ray.origin) && vec3IsValid(ray.direction);
}



// Return the cube mapping of 'v' to 'uv' and 'index' for texture
inline void cubeMap(vec3& v, GLuint *index, vec2 *uv)
{
  GLfloat absX = glm::abs(v.x);
  GLfloat absY = glm::abs(v.y);
  GLfloat absZ = glm::abs(v.z);

  bool isXPositive = v.x > 0;
  bool isYPositive = v.y > 0;
  bool isZPositive = v.z > 0;

  GLfloat maxAxis, uc, vc;

  if (isXPositive && absX >= absY && absX >= absZ) {
	  // u (0 to 1) goes from +z to -z
	  // v (0 to 1) goes from -y to +y
	  maxAxis = absX;
	  uc = -v.z;
	  vc = v.y;
	  *index = 0;
  }
  // NEGATIVE X
  if (!isXPositive && absX >= absY && absX >= absZ) {
	  // u (0 to 1) goes from -z to +z
	  // v (0 to 1) goes from -y to +y
	  maxAxis = absX;
	  uc = v.z;
	  vc = v.y;
	  *index = 1;
  }
  // POSITIVE Y
  if (isYPositive && absY >= absX && absY >= absZ) {
	  // u (0 to 1) goes from -x to +x
	  // v (0 to 1) goes from +z to -z
	  maxAxis = absY;
	  uc = v.x;
	  vc = -v.z;
	  *index = 2;
  }
  // NEGATIVE Y
  if (!isYPositive && absY >= absX && absY >= absZ) {
	  // u (0 to 1) goes from -x to +x
	  // v (0 to 1) goes from -z to +z
	  maxAxis = absY;
	  uc = v.x;
	  vc = v.z;
	  *index = 3;
  }
  // POSITIVE Z
  if (isZPositive && absZ >= absX && absZ >= absY) {
	  // u (0 to 1) goes from -x to +x
	  // v (0 to 1) goes from -y to +y
	  maxAxis = absZ;
	  uc = v.x;
	  vc = v.y;
	  *index = 4;
  }
  // NEGATIVE Z
  if (!isZPositive && absZ >= absX && absZ >= absY) {
	  // u (0 to 1) goes from +x to -x
	  // v (0 to 1) goes from -y to +y
	  maxAxis = absZ;
	  uc = -v.x;
	  vc = v.y;
	  *index = 5;
  }

    // Convert range from -1 to 1 to 0 to 1
    uv->x = 0.5f * (uc / maxAxis + 1.0f);
    uv->y = 0.5f * (vc / maxAxis + 1.0f);
}


inline vec3
getTextureColor(Image *texture, vec2& uv)
{
	if (!texture) {
		return COLOR_WHITE;
	}

	uv = glm::clamp(uv, 0.f + EPSILON, 1.f - EPSILON); // Textures at the edges tend to be not accurate

	int w = texture->getWidth();
	int h = texture->getHeight();

	return texture->getPixel((int)(uv.x * w), (int) (uv.y * h));
}



#endif /* GENERAL_H_ */

