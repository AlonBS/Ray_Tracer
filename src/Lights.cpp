/*
 * Lights.h
 *
 *  Created on: Feb 10, 2018
 *      Author: alonbs
 */


#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/random.hpp>
#include "Lights.h"
#include "General.h"

using namespace glm;


Light::Light(vec3& color)
:_color(color)
{
}


PointLight::PointLight(vec3& color, vec3& pos, mat4& transform)
: Light(color)
{
	printVec3("POSSS 1", pos);
	printMat4("TRANS", transform);
	_position = vec3(transform * vec4(pos, 1.0f));
	printVec3("POSSS 2", _position);
}


DirectionalLight::DirectionalLight(vec3& color, vec3& dir, mat4& transform)
: Light(color)
{
	_direction = normalize(vec3(transform * vec4(dir, 0.0f)));
}


#include "General.h"
#include <stdio.h>

AreaLight::AreaLight(vec3& color,
					 vec3& center,
					 GLfloat& radius,
					 mat4& transform,
					 GLuint samples,
					 bool smartRandom)
: Light(color), _center(center), _radius(radius)
{
	// TODO - REMOVE
	smartRandom = false;

	_positions.push_back(center);

	// First we create a disk that is on XY plane, and then we transform the dots.
	for (int i = 1 ; i < samples ; ++i) {

		if (smartRandom) {

			GLfloat theta = (i*360) / (samples-1);
			theta = glm::radians(theta);
			GLfloat a = static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX / radius);
			vec2 xz;
			xz.x = 0.3 + 0.7*sqrt(a)*cos(radians(theta));
			xz.y = 0.3 + 0.7*sqrt(a)*sin(radians(theta));

			// To account to the translation on the "area" command itself
			_positions.push_back(vec3(xz.x, 0, xz.y));
		}

		else {
			vec2 xz = diskRand(_radius);
			_positions.push_back(vec3(xz.x , 0, xz.y));
		}
	}

	// Apply transforms
	for (vec3 pos : _positions) {
		pos = vec3 (transform * vec4(pos, 1.0f));
		printVec3("POs", pos);
	}
}


