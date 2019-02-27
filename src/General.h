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

using namespace glm;

const GLfloat EPSILON  = 0.0000001f;
//const GLfloat INFINITY = FLT_MAX;
const GLfloat PI = 3.1415926f;
const vec3 COLOR_WHITE = vec3(1.0f, 1.0f, 1.0f);
const vec3 COLOR_BLACK = vec3(0.0f, 0.0f, 0.0f);

using namespace glm;
using namespace std;

inline void
printVec2(const string& name, const vec2& vec)
{
	cout << name << ": " << vec.x << "," << vec.y << endl;
}

inline void
printVec3(const string& name, const vec3& vec)
{
	cout << name << ": " << vec.x << "," << vec.y << "," << vec.z << endl;

//	cout << name << " " << vec.x << " " << vec.y << " " << vec.z << endl;
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
