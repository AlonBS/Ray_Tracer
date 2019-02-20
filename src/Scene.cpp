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

	_objects.clear();
	_textures.clear();
	_pointLights.clear();
	_directionalLights.clear();
}

