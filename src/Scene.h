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
//#include "BoundingVolume.h"
#include "BVH.h"

typedef struct AdditionalRenderParams
{
	bool hardShadows;
	bool flatShading;
	bool noAntiAliasing;

}AdditionalRenderParams;


class Scene {

private:

	AdditionalRenderParams addParams;


	GLuint 							_width, _height;
	GLuint			 				_maxDepth;

	Camera							_camera;

	std::vector<Object*> 	  		_objects;
	std::vector<Mesh*>				_meshes;
	std::vector<Image*>				_textures;
	std::vector<Image*>				_envMaps;
	std::vector<Image*>				_skybox; // Sub-vector of the above to save the need to fetch it everytime
	GLint 							_skyboxIndex;

	std::vector<PointLight*>  		_pointLights;
	std::vector<DirectionalLight*>  _directionalLights;
	std::vector<AreaLight*>  		_areaLights;

	Attenuation_t _attenuation;


	BVH* bvh;


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

	// TODO - FIX
	void addMeshes(vector<Mesh*>& meshes)
	{
		_meshes.reserve(_meshes.size() + meshes.size());
		_meshes.insert(_meshes.end(), meshes.begin(), meshes.end());
	}

	BVH* getBVH() { return bvh;}

	void addTexture(Image *texture) { _textures.push_back(texture); }
	void addTextures(vector<Image*>& textures)
	{
		_textures.reserve(_textures.size() + textures.size());
		_textures.insert(_textures.end(), textures.begin(), textures.end());
	}
	void addEnvMap(Image *envMap) { _envMaps.push_back(envMap); }

	bool hasSkybox() { return _skyboxIndex >= 0;}
	void setSkyBox(GLuint index) { _skybox = getEnvMaps(index); _skyboxIndex = index;}
	Image* getSkyboxTexture(GLuint index) { assert(index < 6); return _skybox[index];}


	Image* getTexture(uint i) { assert(i < _textures.size()); return _textures[i]; }
	vector<Image*> getEnvMaps(uint i) { assert (6*i < _envMaps.size()); return vector<Image*> (_envMaps.begin() + 6*i, _envMaps.begin() + 6*i + 6); }

	void addPointLight(PointLight *light) { _pointLights.push_back(light); }
	std::vector<PointLight*>& getPointLights() { return _pointLights; }

	void addDirectionalLight(DirectionalLight *light) { _directionalLights.push_back(light); }
	std::vector<DirectionalLight*>& getDirectionalLights() { return _directionalLights; }

	void addAreaLight(AreaLight *light) { _areaLights.push_back(light); }
	std::vector<AreaLight*>& getAreaLights() { return _areaLights; }

	Attenuation_t& attenuation() { return _attenuation; }

	void constructAccelerationStructures();

	void handleAdditionalParams(AdditionalRenderParams& params);
	bool noAntiAliasing() { return addParams.noAntiAliasing; }


};

#endif /* SCENE_H_ */


