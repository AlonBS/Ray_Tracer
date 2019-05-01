/*
 * Scene.cpp
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#include "Scene.h"

Scene::Scene() {

	_skyboxIndex = -1;
}

Scene::~Scene() {

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

	for (auto* e : _envMaps) {
		delete e;
	}

	for (auto* p : _pointLights) {
		delete p;
	}

	for (auto* d : _directionalLights) {
		delete d;
	}

	for (auto* a : _areaLights) {
		delete a;
	}


	_objects.clear();
	_meshes.clear();
	_textures.clear();
	_envMaps.clear();
	_skybox.clear();
	_pointLights.clear();
	_directionalLights.clear();
	delete (bvh); bvh = nullptr;
}

void Scene::handleAdditionalParams(AdditionalRenderParams& params)
{
	addParams = params;

	if (addParams.flatShading) {
		Triangle::setFaceNormal();
	}
}


void Scene::constructAccelerationStructures()
{
	bvh = new BVH(_meshes);
}

