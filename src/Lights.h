/*
 * Lights.h
 *
 *  Created on: Feb 10, 2018
 *      Author: alonbs
 */

#ifndef LIGHTS_H_
#define LIGHTS_H_

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>

using namespace glm;

typedef struct Attenuation {

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

}Attenuation_t;

class Light {

public:

	vec3 _color;
	GLfloat _intensity;

	Light(vec3& color, GLfloat& intensity);
	virtual ~Light() {};

};

class PointLight : public Light {

public:
	vec3 _position;

	PointLight(vec3& color, GLfloat& intensity, vec3& pos, mat4 & transform);
	virtual ~PointLight() {}
};

class DirectionalLight : public Light {

public:
	vec3 _direction;


	DirectionalLight(vec3& color, GLfloat& intensity, vec3& dir, mat4& transform);
	virtual ~DirectionalLight() {}
};


class AreaLight : public Light {

public:
	vec3 _center;
	GLfloat _radius;

	std::vector<vec3> _positions;


	AreaLight(
			vec3& color,
			GLfloat& intensity,
			vec3& _center,
			GLfloat& radius,
			mat4& transform,
			GLuint samples,
			bool smartRandom=true);

	virtual ~AreaLight() {}
};





#endif /* LIGHTS_H_ */
