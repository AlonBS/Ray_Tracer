/*
 * RayTrace.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>

#include "RayTracer.h"
#include "General.h"


RayTracer::RayTracer()
{
}

RayTracer::~RayTracer() {
}


Image* RayTracer::rayTraceMT(Scene& scene)
{
	Image *image = new Image(scene.width(), scene.height());

	size_t max = scene.width() * scene.height();
	size_t cores = std::thread::hardware_concurrency() / 2; // to not burn cpu.
	/*volatile*/ atomic<GLuint64> count(0);
	vector<future<void>> future_vector;

	while (cores--)
	{
	    future_vector.push_back(
	        std::async(launch::async, [=, &scene, &count]()
	        {
	            while (true)
	            {
	                std::size_t index = count++;
	                if (index >= max)
	                    break;

	                GLuint i = index % scene.width();
	                GLuint j = index / scene.width();

	                Ray ray = scene.camera().generateRay(i + .5, j - .5);
	                vec3 color = recursiveRayTrace(scene, ray, scene.maxDepth());
	                image->setPixel(i, j, color);
	            }
	        }));
	}

	// To display progress bar, it's sufficient to 'actively' wait for one of the working threads
	std::future_status status;
	std::future<void>& f = future_vector[0];
	do {
		status = f.wait_for(std::chrono::milliseconds(100));
		if (status == std::future_status::timeout) {

			std::cout << "\tProgress: [ "<< setprecision(1) << fixed << (count / (GLfloat)max) * 100.0 << "% ] \r";
			std::cout.flush();
		}
	} while (status != std::future_status::ready);


	for (auto& e : future_vector) {
		e.get();
	}

	return image;
}


// single threaded - for benchmark purposes and debugging
Image* RayTracer::rayTraceST(Scene& scene)
{
	Image *image = new Image(scene.width(), scene.height());
	vec3 color;

	for (GLuint i = 0 ; i < scene.width() ; ++i)
	{
		for (GLuint j = 0 ; j < scene.height(); ++j)
		{
			Ray ray = scene.camera().generateRay(i + .5, j - .5);
			color = recursiveRayTrace(scene, ray, scene.maxDepth());
			image->setPixel(i, j, color);

		}

		if (i % 20 == 0) {
			cout << "\tProgress: [ "<< setprecision(1) << fixed << (i / (GLfloat)scene.width()) * 100.0 << "% ] \r";
			cout.flush();
		}
	}

	return image;
}


vec3 RayTracer::recursiveRayTrace(Scene& scene, Ray & ray, GLuint depth)
{
	vec3 color = COLOR_BLACK;

	++rayTracerStats.numOfRays;

	if (depth == 0) {
		return COLOR_BLACK;
	}
	if (!rayIsValid(ray)) {
		return COLOR_BLACK;
	}

	Intersection hit = intersectScene(scene, ray);
	if (!hit.isValid) {
		return COLOR_BLACK;
	}

	color = computeLight(scene, ray, hit);

	vec3 reflectedRayOrigin = hit.point;
	vec3 reflectedRayDir = glm::reflect(ray.direction, hit.normal);
	reflectedRayOrigin = reflectedRayOrigin + EPSILON * reflectedRayDir;
	Ray reflectedRay(reflectedRayOrigin , reflectedRayDir);

	return color + hit.properties._specular * recursiveRayTrace(scene, reflectedRay, --depth);

}


bool RayTracer::rayIsValid(const Ray& ray)
{
	// If any of the components of the ray is invalid - we declare the whole ray as invalid
	if (isnan(ray.origin.x) || isnan(ray.origin.y) || isnan(ray.origin.z)  ||
	    isnan(ray.direction.x) || isnan(ray.direction.y) || isnan(ray.direction.z) )
	{
		return false;
	}

	return true;
}


Intersection RayTracer::intersectScene(Scene & scene, Ray& ray)
{
	GLfloat minDist = INFINITY;
	GLfloat dist, bboxDist;
	vec3 point;
	vec3 normal;
	ObjectTexColors texColors;
	ObjectProperties objProps;

	Intersection hit;
	hit.isValid = false;


	// Currently - primitive objects are not divided within the bounding volume hierarchies.
	// And so, we first search for an intersection with any of
	// the primitives (which is rather quickly, since it's a single function call (and also BBox optimized)
	// and then we move to the complex objects (models), and we can use the closest intersection when testing
	// against extents (and not meshes).
	for (Object *object : scene.getObjects()) {

		if (object->bBoxIntersectsRay(ray, &bboxDist)) {

			// If we already found a closer object than this object bbox - no point in checking this object
			if (minDist < bboxDist)
				continue;

			if (object->intersectsRay(ray, &dist, &point, &normal, &texColors, &objProps)) {

				if (dist < minDist) {

					minDist = dist;
					hit.point = point;
					hit.normal = normal;
					hit.texColors = texColors;

					hit.properties = objProps;
					hit.isValid = true;
				}
			}
		}
	}


	// Now we check for complex object, where we already know the closest primitive object for this ray.
	if (scene.getBVH()->intersectsRay(ray, minDist, &dist, &point, &normal, &texColors, &objProps))
	{
		hit.point = point;
		hit.normal = normal;
		hit.texColors = texColors;

		hit.properties = objProps;
		hit.isValid = true;
	}

	return hit;
}


vec3 RayTracer::computeLight(Scene& scene, Ray& r, Intersection& hit)
{
	vec3 color = COLOR_BLACK, tempColor;

	Ray shadowRay;
	vec3 srOrigin;
	vec3 srDir;
	GLfloat distToLight;

	vec3 eyeDir;
	vec3 halfAng;

	// The 'eye' direction is where the current ray was shot from, and hit.
	eyeDir = normalize(r.origin - hit.point);


	// Add point lights
	for (PointLight* p : scene.getPointLights()) {

		srDir = normalize(p->_position - hit.point);
		srOrigin = hit.point + 10 * EPSILON * srDir; // Move a little to avoid floating point errors
		shadowRay = Ray(srOrigin, srDir);
		distToLight = length(p->_position - hit.point);

		if (isVisibleToLight(scene, shadowRay, distToLight)) {

			halfAng = normalize(srDir + eyeDir);

			tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, srDir, hit.normal, halfAng);
			// take attenuation into account
			GLfloat atten = 1 / (scene.attenuation().constant + scene.attenuation().linear * distToLight + scene.attenuation().quadratic * distToLight * distToLight);
			tempColor *= atten;
			color += tempColor;
		}
	}

	// Add area lights
	for (AreaLight* p : scene.getAreaLights()) {

		for (vec3& pos : p->_positions) {

			srDir = normalize(pos - hit.point);
			srOrigin = hit.point + 10 * EPSILON * srDir; // Move a little to avoid floating point errors
			shadowRay = Ray(srOrigin, srDir);
			distToLight = length(pos - hit.point);

			if (isVisibleToLight(scene, shadowRay, distToLight)) {

				halfAng = normalize(srDir + eyeDir);

				tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, srDir, hit.normal, halfAng);
				// take attenuation into account
				GLfloat atten = 1 / (scene.attenuation().constant + scene.attenuation().linear * distToLight + scene.attenuation().quadratic * distToLight * distToLight);
				tempColor *= atten;
				tempColor /= p->_positions.size();
				color += tempColor;
			}

		}


	}


	for (DirectionalLight* p : scene.getDirectionalLights()) {

		srDir = normalize(p->_direction);
		srOrigin = hit.point + 10 * EPSILON * srDir; // Move a little to avoid floating point errors
		shadowRay = Ray(srOrigin, srDir);
		distToLight = INFINITY;


		if (isVisibleToLight(scene, shadowRay, distToLight)) {

			halfAng = normalize(srDir + eyeDir);
			tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, srDir, hit.normal, halfAng);

			color += tempColor;
		}
	}


	//Ambient & Emission - regardless of lights
	color += hit.properties._ambient * hit.texColors._ambientTexColor +
			 hit.properties._emission * hit.texColors._ambientTexColor; // TODO Currently - ambient texture == emission texture
	return color;
}


bool RayTracer::isVisibleToLight(Scene& scene, Ray& shadowRay, GLfloat limit)
{
	GLfloat dist;
	GLfloat bboxDist;

	for (Object * o : scene.getObjects()) {

		if (o->bBoxIntersectsRay(shadowRay, &bboxDist)) {

			// The light is closer from hit point and object BBox. So no point in checking.
			if (limit < bboxDist)
				continue;

			if (o->intersectsRay(shadowRay, &dist, nullptr, nullptr, nullptr, nullptr)) {

				// If there's a intersection to a object which is within limit (no 'after' the light)
				// then there's no visibility
				if (dist < limit) {
					return false;
				}
			}
		}

	}

	// Now we check for complex object, where we already know the closest primitive object for this ray.
	if (scene.getBVH()->intersectsRay(shadowRay, limit, &dist, nullptr, nullptr, nullptr, nullptr))
	{
		return false;
	}

	return true;
}

vec3 RayTracer::__blinn_phong(const ObjectProperties& objProps,
							  const ObjectTexColors& objTexColors,
							  const vec3& lightColor,
							  const vec3& lightDir,
							  const vec3& normal,
							  const vec3& halfAng)
{

	// diffuse
	GLfloat diff = glm::max(dot(normal, lightDir), 0.0f);
	vec3 diffuse = diff * objProps._diffuse * objTexColors._diffuseTexColor;

	// Specular
	GLfloat spec = glm::pow(glm::max(dot(halfAng, normal), 0.0f), objProps._shininess);
	vec3 specular = spec * objProps._specular * objTexColors._specularTexColor;

	return (diffuse + specular) * lightColor;
}

