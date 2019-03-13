/*
 * Scene.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "Object.h"
#include "Lights.h"
#include "Camera.hpp"
#include "BoundingVolume.h"


class Scene {

private:

	GLuint 							_width, _height;
	GLuint			 				_maxDepth;

	Camera							_camera;

	std::vector<Object*> 	  		_objects;
	std::vector<Image*>				_textures;

	std::vector<PointLight*>  		_pointLights;
	std::vector<DirectionalLight*>  _directionalLights;

	Attenuation_t _attenuation;



	std::vector<BoundingVolume*>	_boundingVolumes;


public:

	Scene();
	virtual ~Scene();

	GLuint& width() { return _width;}
	const GLuint& width() const { return _width;}
	GLuint& height() { return _height;}
	const GLuint& height() const { return _height;}

	GLuint& maxDepth() { return _maxDepth;}
	const GLuint& maxDepth() const { return _maxDepth;}

	Camera& camera() {return _camera;}


	void addObject(Object *obj) { _objects.push_back(obj); }
	std::vector<Object*>& getObjects() { return _objects; }

	void addTexture(Image *texture) { _textures.push_back(texture); }
	Image* getTexture(uint i) { assert(i < _textures.size()); return _textures[i]; }

	void addPointLight(PointLight *light) { _pointLights.push_back(light); }
	std::vector<PointLight*>& getPointLights() { return _pointLights; }

	void addDirectionalLight(DirectionalLight *light) { _directionalLights.push_back(light); }
	std::vector<DirectionalLight*>& getDirectionalLights() { return _directionalLights; }

	Attenuation_t& attenuation() { return _attenuation; }


	std::vector<BoundingVolume*>& getBoundingVolumes() { return _boundingVolumes; }
	void constructAccelerationStructures();


};

#endif /* SCENE_H_ */


