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




class Scene {

private:

	std::vector<Object*> 	  		objects;
	std::vector<Image*>				textures;

	std::vector<PointLight*>  		pointLights;
	std::vector<DirectionalLight*>  directionalLights;


	Attenuation_t attenuation;


public:

	Scene();
	virtual ~Scene();

	void addObject(Object *obj) { objects.push_back(obj); }
	void addTexture(Image *texture) { textures.push_back(texture); }

	void addPointLight(PointLight *light) { pointLights.push_back(light); }
	void addDirectionalLight(DirectionalLight *light) { directionalLights.push_back(light); }

	//void setAttenuation(Attenuation& atten) { this->attenuation = atten; }


	std::vector<Object*>& getObjects() { return objects; }
	std::vector<PointLight*>& getPointLights() { return pointLights; }
	std::vector<DirectionalLight*>& getDirectionalLights() { return directionalLights; }

	Image* getTexture(int i) { assert(i >= 0 && i < textures.size()); return textures[i]; }

	Attenuation_t& Attenuation() { return attenuation; }

};

#endif /* SCENE_H_ */
