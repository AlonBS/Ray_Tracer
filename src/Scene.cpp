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

	for (auto* o : objects) {
		delete o;
	}

	for (auto* t : textures) {
		delete t;
	}

	for (auto* p : pointLights) {
		delete p;
	}

	for (auto* d : directionalLights) {
		delete d;
	}

	objects.clear();
	textures.clear();
	pointLights.clear();
	directionalLights.clear();
}

