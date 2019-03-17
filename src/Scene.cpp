/*
 * Scene.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#include "Scene.h"

Scene::Scene() {
	// TODO Auto-generated constructor stub

}

Scene::~Scene() {

	for (auto* bv : _boundingVolumes) {
		delete bv;
	}

	for (auto* o : _objects) {
		delete o;
	}

	for (auto* t : _textures) {
		delete t;
	}

	for (auto* p : _pointLights) {
		delete p;
	}

	for (auto* d : _directionalLights) {
		delete d;
	}

	_boundingVolumes.clear();
	_objects.clear();
	_textures.clear();
	_pointLights.clear();
	_directionalLights.clear();
}


void Scene::constructAccelerationStructures()
{
	// TODO - this will be update with Oct-tree on next stages

	for (Object* obj : _objects)
	{

		_boundingVolumes.push_back(new BoundingVolume(obj));

	}
}

