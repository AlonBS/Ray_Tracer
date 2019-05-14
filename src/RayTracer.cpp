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

#define ANTI_ALIASING_NUM_OF_SAMPLES 16 // Cook's 'magic' number
#define ANTI_ALIASING_DIST_MIN_VALUE 0
#define ANTI_ALIASING_DIST_MAX_VALUE 1


#define REFL_PERT_NUM_OF_RAYS 8    // The number of perturbed ray to generate when reflecting
#define REFL_PERT_DIST_MIN_VALUE 0 // The minimal value to use for the uniform distribution of the perturbation of reflection
#define REFL_PERT_DIST_MAX_VALUE 1 // the maximal.

#define REFR_PERT_NUM_OF_RAYS 8    // The number of perturbed ray to generate when refracting
#define REFR_PERT_DIST_MIN_VALUE 0 // Of refration
#define REFR_PERT_DIST_MAX_VALUE 1 //


RayTracer::RayTracer()
{
	random_device rd;  //Will be used to obtain a seed for the random number engine
	generator = mt19937(rd());

	AA_dis = uniform_real_distribution<> (ANTI_ALIASING_DIST_MIN_VALUE, ANTI_ALIASING_DIST_MAX_VALUE);
	refl_dis = uniform_real_distribution<> (REFL_PERT_DIST_MIN_VALUE, REFL_PERT_DIST_MAX_VALUE);
	refr_dis = uniform_real_distribution<> (REFR_PERT_DIST_MIN_VALUE, REFR_PERT_DIST_MAX_VALUE);
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

	                if (scene.noAntiAliasing())
	                {
	                	Ray ray = scene.camera().generateRay(i + .5, j - .5);
	                	vec3 color = recursiveRayTrace(scene, ray, scene.maxDepth());
	                	image->setPixel(i, j, color);


	                }

	                // Anti Aliasing
	                else {
	                	vec3 color = COLOR_BLACK;
	                	GLfloat delta;

	                	for (int x = 0 ; x < ANTI_ALIASING_NUM_OF_SAMPLES ; ++x) {

	                		delta = AA_dis(generator);
	                		Ray ray = scene.camera().generateRay(i + delta, j - delta);
	                		color += recursiveRayTrace(scene, ray, scene.maxDepth());
	                	}

	                	color /= ANTI_ALIASING_NUM_OF_SAMPLES;
	                	image->setPixel(i, j, color);
	                }
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

			if (scene.noAntiAliasing())
			{
				Ray ray = scene.camera().generateRay(i + .5, j - .5);
				vec3 color = recursiveRayTrace(scene, ray, scene.maxDepth());
				image->setPixel(i, j, color);
			}

			// Anti Aliasing
			else {

				vec3 color = COLOR_BLACK;
				GLfloat delta;

				for (int x = 0 ; x < ANTI_ALIASING_NUM_OF_SAMPLES ; ++x) {

					delta = AA_dis(generator);
					Ray ray = scene.camera().generateRay(i + delta, j - delta);
					color += recursiveRayTrace(scene, ray, scene.maxDepth());
				}

				color /= ANTI_ALIASING_NUM_OF_SAMPLES;
				image->setPixel(i, j, color);
			}

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

//	vec3 o = vec3(0,0,10);
//	vec3 d = vec3(0,0,-1);
//	ray = Ray(o,d);

	Intersection hit = intersectScene(scene, ray);
	if (!hit.isValid) {

		if (scene.hasSkybox()) {

			GLuint index;
			vec2 uv;
			cubeMap(ray.direction, &index, &uv);
			return getTextureColor(scene.getSkyboxTexture(index), uv);
		}
		else {
			return COLOR_BLACK;
		}
	}

	color += calculateReflectionsNRefractions(scene, ray, hit, depth);
	color += computeLight(scene, ray, hit);

	return color;
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
	for (const auto& object : scene.getObjects()) {

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

//	printVec3("Norm", hit.normal);

	// The 'eye' direction is where the current ray was shot from, and hit.
	eyeDir = normalize(r.origin - hit.point);

	//Ambient & Emission - regardless of lights
	color += hit.properties._ambient * hit.texColors._ambientTexColor +
			 hit.properties._emission * hit.texColors._ambientTexColor; // TODO Currently - ambient texture == emission texture


	if (equalToVec3(hit.properties._diffuse, NEGLIGENT_CONTRIBUTION) &&
		equalToVec3(hit.properties._specular, NEGLIGENT_CONTRIBUTION) ) {
		return color;
	}

	// Add point lights
	for (PointLight* p : scene.getPointLights()) {

		srDir = normalize(p->_position - hit.point);
		srOrigin = hit.point + 10 * EPSILON * srDir; // Move a little to avoid floating point errors
		shadowRay = Ray(srOrigin, srDir);
		distToLight = length(p->_position - hit.point);

		if (isVisibleToLight(scene, shadowRay, distToLight)) {

			halfAng = normalize(srDir + eyeDir);

			tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, p->_intensity, srDir, hit.normal, halfAng);
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

				tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, p->_intensity, srDir, hit.normal, halfAng);
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
			tempColor = __blinn_phong(hit.properties, hit.texColors, p->_color, p->_intensity, srDir, hit.normal, halfAng);

			color += tempColor;
		}
	}

	return color;
}


bool RayTracer::isVisibleToLight(Scene& scene, Ray& shadowRay, GLfloat limit)
{
	GLfloat dist;
	GLfloat bboxDist;

	for (auto& o : scene.getObjects()) {

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
	if (scene.getBVH()->intersectsRay(shadowRay, limit, &dist, nullptr, nullptr, nullptr, nullptr, true))
	{
		return false;
	}

	return true;
}

vec3 RayTracer::__blinn_phong(const ObjectProperties& objProps,
							  const ObjectTexColors& objTexColors,
							  const vec3& lightColor,
							  const GLfloat& lightIntensity,
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

	return (diffuse + specular) * lightColor * lightIntensity;
}



vec3
RayTracer::calculateReflectionsNRefractions(Scene& scene,
											Ray& ray,
											Intersection& hit,
											GLuint depth)
{

	vec3 reflectionColor = COLOR_BLACK;
	vec3 refractionColor = COLOR_BLACK;

	GLfloat kr = 1.f;

	// object is refractive
	if (!equalToVec3(hit.properties._refraction, COLOR_BLACK)){

		kr = computeFrenselProportion(ray.direction, hit.normal, hit.properties._refractionIndex);

		// Not total internal reflection
		if (kr < 1.0f) {
			refractionColor = calculateRefractions(scene, ray.direction, hit, depth);
		}

	}

	if (!equalToVec3(hit.properties._reflection, COLOR_BLACK)){

		reflectionColor = calculateReflections(scene, ray.direction, hit, depth);
	}

	return kr*reflectionColor + (1.f-kr)*refractionColor;

}



GLfloat
RayTracer::computeFrenselProportion(const vec3& I, const vec3& N, const GLfloat& ior)
{
	GLfloat cosi = dot(I, N);
	GLfloat etai = 1, etat = ior;
	if (cosi > 0) {
		std::swap(etai, etat);
	}

	// Compute sini using Snell's law
	GLfloat sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi*cosi));
	// Total internal reflection
	if (sint >= 1 - EPSILON) {
		return 1;
	}
	else {
		GLfloat cost = sqrtf(std::max(0.f, 1 - sint*sint));
		cosi = fabsf(cosi);
		GLfloat Rs = ((etat*cosi) - (etai*cost)) / ((etat*cosi) + (etai*cost));
		GLfloat Rp = ((etai*cosi) - (etat*cost)) / ((etai*cosi) + (etat*cost));
		return (Rs*Rs + Rp*Rp) / 2;
	}
}



void RayTracer::orthoBasis(const vec3& x,
						   vec3& u,
						   vec3& v,
						   vec3& w)
{
	u = x;
	v = normalize(vec3(0, -x.z, x.y));
	if (equalToVec3(v, vec3(0,0,0))) {
		v = normalize(vec3(-x.z, 0, x.x));
	}

	w = normalize(cross(u,v));
}


vec3
RayTracer::calculateReflections(Scene& scene,
								vec3& rayDir,
								Intersection& hit,
								GLuint depth)
{
	// object is not reflective
	if (equalToVec3(hit.properties._reflection, COLOR_BLACK)){
		return COLOR_BLACK;
	}


	vec3 reflectedRayOrigin = hit.point;
	vec3 reflectedRayDir = normalize(glm::reflect(rayDir, hit.normal));
	reflectedRayOrigin = reflectedRayOrigin + EPSILON * reflectedRayDir;
	Ray reflectedRay(reflectedRayOrigin , reflectedRayDir);

	if (hit.properties._reflectionBlur > 0) {

		vec3 color = COLOR_BLACK;
		vec3 u,v,w;
		orthoBasis(reflectedRayDir, u, v, w);

		// color = recursiveRayTrace(scene, reflectedRay, depth-1); - This is left out because it gives more realistic results.
		for (GLuint n = 0 ; n < REFL_PERT_NUM_OF_RAYS ; ++n)
		{
			GLfloat w1 = hit.properties._reflectionBlur * refl_dis(generator);
			GLfloat w2 = hit.properties._reflectionBlur * refl_dis(generator);

			vec3 perturbedRayDir = normalize(u + w1*v + w2*w);

			GLfloat contribution = dot(reflectedRayDir, perturbedRayDir);
			Ray perturbedRay(reflectedRayOrigin , perturbedRayDir);

			color += contribution * recursiveRayTrace(scene, perturbedRay, depth-1);
		}


		return (hit.properties._reflection * color) / (GLfloat) REFL_PERT_NUM_OF_RAYS;
	}

	else {

		return hit.properties._reflection * recursiveRayTrace(scene, reflectedRay, depth-1);;
	}
}



vec3
RayTracer::calculateRefractions(Scene& scene,
								vec3& rayDir,
								Intersection& hit,
								GLuint depth)
{
	// object is not refractive
	if (equalToVec3(hit.properties._refraction, COLOR_BLACK)){
		return COLOR_BLACK;
	}

	vec3 dir;
	vec3 norm;
	GLfloat eta;

	GLfloat nDotD = dot(hit.normal, rayDir);

	if (nDotD > EPSILON) { // Going out of the object
		dir = rayDir;
		norm = -hit.normal;
		eta = hit.properties._refractionIndex / VOID_INDEX;
	}
	else {
		dir = rayDir; // TODO - consider -rayDir
		norm = hit.normal;
		eta = VOID_INDEX / hit.properties._refractionIndex;
	}

	vec3 refractedRayOrigin = hit.point;
	vec3 refractedRayDir = normalize(glm::refract(dir, norm, eta));


	refractedRayOrigin = refractedRayOrigin + EPSILON * refractedRayDir;
	Ray refractedRay(refractedRayOrigin , refractedRayDir);

	if (hit.properties._refractionBlur > 0) {

		vec3 color = COLOR_BLACK;
		vec3 u,v,w;
		orthoBasis(refractedRayDir, u, v, w);

		// color = recursiveRayTrace(scene, refractedRayOrigin, depth-1); - This is left out because it gives more realistic results.
		for (GLuint n = 0 ; n < REFR_PERT_NUM_OF_RAYS ; ++n)
		{
			GLfloat w1 = hit.properties._refractionBlur * refr_dis(generator);
			GLfloat w2 = hit.properties._refractionBlur * refr_dis(generator);

			vec3 perturbedRayDir = normalize(u + w1*v + w2*w);

			GLfloat contribution = dot(refractedRayDir, perturbedRayDir);
			Ray perturbedRay(refractedRayOrigin , perturbedRayDir);

			color += contribution * recursiveRayTrace(scene, perturbedRay, depth-1);
		}


		return (hit.properties._refraction * color) / (GLfloat) REFR_PERT_NUM_OF_RAYS;
	}

	else {

		return hit.properties._refraction * recursiveRayTrace(scene, refractedRay, depth-1);
	}

}

