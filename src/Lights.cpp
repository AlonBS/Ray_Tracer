/*
 * Lights.h
 *
 *  Created on: Feb 10, 2018
 *      Author: alonbs
 */


#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Lights.h"
#include <glm/gtc/random.hpp>

using namespace glm;


Light::Light(vec3& color)
:_color(color)
{
}


PointLight::PointLight(vec3& color, vec3& pos)
: Light(color), _position(pos)
{
}


DirectionalLight::DirectionalLight(vec3& color, vec3& dir)
: Light(color), _direction(normalize(dir))
{
}


#include "General.h"
#include <stdio.h>

AreaLight::AreaLight(vec3& color,
					 vec3& center,
					 GLfloat& radius,
					 GLuint samples,
					 bool randomSamples)
: Light(color), _center(center), _radius(radius)
{

	_positions.push_back(center);

	for (int i = 1 ; i < samples ; ++i) {

		if (randomSamples) {
			vec2 xz = glm::diskRand(_radius);
			_positions.push_back(vec3(xz.x, center.y, xz.y));

		}

		else {

			GLfloat theta = (i*360) / (samples-1);
			theta = glm::radians(theta);
			GLfloat a = static_cast <GLfloat> (rand()) / static_cast <GLfloat> (RAND_MAX / radius);
			vec2 xz;
			xz.x = 0.3 + 0.7*sqrt(a)*glm::cos(glm::radians(theta));
			xz.y = 0.3 + 0.7*sqrt(a)*glm::sin(glm::radians(theta));

			_positions.push_back(vec3(xz.x, center.y, xz.y));
		}
	}
}


