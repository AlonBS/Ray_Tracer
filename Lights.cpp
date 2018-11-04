/*
 * Lights.h
 *
 *  Created on: Feb 10, 2018
 *      Author: alonbs
 */


#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Lights.h"

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
