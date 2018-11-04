/*
 * RenderInfo.h
 *
 *  Created on: Jan 18, 2018
 *      Author: alonbs
 */

#ifndef RENDERINFO_H_
#define RENDERINFO_H_

#include <GL/glew.h>
#include "Camera.h"
#include "Scene.h"

//#define PRINT_VEC3(name, x) std:cout << name << ":(" << x.x << "," << x.y << "," << x.z << ")" << std::endl;



struct RenderInfo {

	GLuint width;
	GLuint height;

	GLuint maxDepth;

	std::string outputFile;

	Camera camera;

	Scene scene;

	vector<glm::vec3> vertices;
	vector<glm::vec3> verticesNormals;

	vector<glm::vec3> verticesTexV; // For Vertices with textures mapping
	vector<glm::vec2> verticesTexT; // The texture mapping

	Image *boundTexture;
	bool textureIsBound;

};




#endif /* RENDERINFO_H_ */
