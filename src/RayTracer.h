/*
 * RayTrace.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef RAYTRACER_H_
#define RAYTRACER_H_

#include <vector>
#include <future>

#include "GL/glew.h"
#include "Image.h"
#include "Scene.h"
#include "Ray.h"
#include "Camera.hpp"




class RayTracer {
public:

	RayTracer();
	virtual ~RayTracer();

	Image* rayTraceMT(Scene& scene);

	Image* rayTraceST(Scene& scene);




private:


	vec3 recursiveRayTrace(Scene& scene, Ray& ray, GLuint depth);

	bool rayIsValid(const Ray& ray);

	Intersection intersectScene(Scene & scene, Ray& ray);

	vec3 computeLight(Scene & scene, Ray& r, Intersection& hit);

	bool isVisibleToLight(Scene& scene, Ray& shadowRay, GLfloat limit);


	vec3 __blinn_phong(const ObjectProperties& objProps,
			  	  	   const ObjectTexColors& objTexColors,
					   const vec3& lightColor,
					   const vec3& lightDir,
					   const vec3& normal,
					   const vec3& halfAng);


	vec3 calculateReflrectionsNRefractions(Scene& scene, Ray& ray, Intersection& hit, GLuint depth);

	GLfloat computeFrenselProportion(const vec3& I, const vec3& N, const GLfloat& eta);

	vec3 calculateReflections(Scene& scene, Ray& ray, Intersection& hit, GLuint depth);
	vec3 calculateRefractions(Scene& scene, Ray& ray, Intersection& hit, GLuint depth);

};



#endif /* RAYTRACER_H_ */
