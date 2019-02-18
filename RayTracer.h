/*
 * RayTrace.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include "GL/glew.h"
#include "Image.h"
#include "Scene.h"
#include "Camera.h"
#include "Ray.h"

#include <vector>


typedef struct Intersection {

	bool isValid;

	vec3 point;
	vec3 normal;
	ObjectTexColors texColors;

	ObjectProperties properties; // The object's properties at intersection

}Intersection;


class RayTracer {
public:
	RayTracer();
	virtual ~RayTracer();


	Image* rayTrace(Camera& camera, Scene& scene, GLuint width, GLuint height, GLuint maxDepth);

private:

	vec3 recursiveRayTrace(Scene& scene, Ray& ray, GLuint depth);

	Intersection intersectScene(Scene & scene, Ray& ray);

	vec3 computeLight(Scene & scene, Ray& r, Intersection& hit);

	bool isVisibleToLight(vector<Object*>& objects, Ray& shadowRay, GLfloat limit);


	vec3 __blinn_phong(const ObjectProperties& objProps,
			  	  	   const ObjectTexColors& objTexColors,
					   const vec3& lightColor,
					   const vec3& lightDir,
					   const vec3& normal,
					   const vec3& halfAng);

};



#endif /* RAYTRACER_H_ */
