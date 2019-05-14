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
#include "BVH.h"

typedef struct AdditionalRenderParams
{
	bool hardShadows;
	bool flatShading;
	bool noAntiAliasing;
	bool noBumpMaps;

}AdditionalRenderParams;


class Scene {

private:

	AdditionalRenderParams addParams;

	GLuint 												_width, _height;
	GLuint			 									_maxDepth;

	unique_ptr<const Camera>				_camera;
	vector<unique_ptr<const Object>> 	  	_objects;

	vector<shared_ptr<const Mesh>>			_meshes;
	vector<shared_ptr<const Image>>			_textures;
	vector<shared_ptr<const Image>>			_envMaps;
	vector<const Image*>					_skybox; // Sub-vector of the above to save the need to fetch it everytime
	GLint 								    _skyboxIndex;

	vector<PointLight*>  		_pointLights;
	vector<DirectionalLight*>  _directionalLights;
	vector<AreaLight*>  		_areaLights;

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

	void addCamera(unique_ptr<Camera> camera) { _camera = move(camera);}

	auto camera() const -> const Camera& {return *_camera.get(); }


	void addObject(unique_ptr<Object>& obj) { _objects.push_back(move(obj)); }
	const vector<unique_ptr<const Object>>& getObjects() const { return _objects; }

	void addMeshes(vector<shared_ptr<const Mesh>>& meshes)
	{
		_meshes.reserve(_meshes.size() + meshes.size());
		_meshes.insert(_meshes.end(), make_move_iterator(meshes.begin()), make_move_iterator(meshes.end()));
	}

	BVH* getBVH() { return bvh;}

	void addTexture(shared_ptr<Image>& texture) { _textures.push_back(move(texture)); }
	shared_ptr<const Image> getTexture(uint i) const { assert(i < _textures.size()); return _textures[i]; }

	void addEnvMap(shared_ptr<Image>& envMap) { _envMaps.push_back(move(envMap)); }
	vector<shared_ptr<const Image>> getEnvMapsImages(uint i) const
	{
		return vector<shared_ptr<const Image>>(_envMaps.begin() +6*i, _envMaps.end() + 6*i + 6);
//		assert (6*i < _envMaps.size());
//		vector<const Image*> retVal{};
//		for (int j = 6*i ; j < 6*i+6 ; ++j) {
//			retVal.push_back(_envMaps[i].get());
//		}
//		return retVal;
	}

	bool hasSkybox() const { return _skyboxIndex >= 0;}
	void setSkyBox(GLuint index)
	{
		for (const auto & emi: getEnvMapsImages(index)) {
			_skybox.push_back(emi.get());
		}
		_skyboxIndex = index;
	}

	const Image* getSkyboxTexture(GLuint index) const
	{
		assert(index < 6);
		return _skybox[index];
	}



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


