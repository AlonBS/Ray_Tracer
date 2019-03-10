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

	Image* rayTraceMT(Scene& scene);

	Image* rayTraceST(Scene& scene);




private:


	vec3 recursiveRayTrace(Scene& scene, Ray& ray, GLuint depth);

	bool rayIsValid(const Ray& ray);

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
