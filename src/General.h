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



using namespace glm;
using namespace std;

typedef struct ObjectProperties {

	vec3 _ambient;
	vec3 _emission;
	vec3 _diffuse;
	vec3 _specular;
	GLfloat _shininess;

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
//    glm::vec3 Tangent;
//    // bitangent
//    glm::vec3 Bitangent;
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

void clearStats();

#define IN
#define OUT


//const GLfloat EPSILON  = 0.00000000001f;
const GLfloat EPSILON  = 0.0001f;
//const GLfloat INFINITY = FLT_MAX;
const GLfloat PI = 3.1415926f;
const vec3 COLOR_WHITE = vec3(1.0f, 1.0f, 1.0f);
const vec3 COLOR_BLACK = vec3(0.0f, 0.0f, 0.0f);


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
epsilonCompareVec3(const vec3& v1, const vec3& v2)
{
	return glm::abs(v1.x - v2.x) < EPSILON &&
		   glm::abs(v1.y - v2.y) < EPSILON &&
		   glm::abs(v1.z - v2.z) < EPSILON;
}




#endif /* GENERAL_H_ */

