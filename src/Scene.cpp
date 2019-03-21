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

//	for (auto* bv : _boundingVolumes) {
//		delete bv;
//	}

	for (auto* o : _objects) {
		delete o;
	}

	// TODO - fix
	for (auto* m : _meshes) {
			delete m;
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


	//_boundingVolumes.clear();
	_objects.clear();
	_meshes.clear();
	_textures.clear();
	_pointLights.clear();
	_directionalLights.clear();
	delete (bvh); bvh = nullptr;
}


void Scene::constructAccelerationStructures()
{
	// TODO - this will be update with Oct-tree on next stages
	bvh = new BVH(_meshes);
}

