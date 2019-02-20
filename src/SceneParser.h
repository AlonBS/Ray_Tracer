// Readfile definitions


#ifndef __SCENE_PARSER__
#define __SCENE_PARSER__

#include <GL/glew.h>

#include "Camera.h"
#include "RenderInfo.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <stack>

using namespace std;

#define MAX_POSSIBLE_VALUES 10

typedef enum CommandType {

	GENERAL,
	CAMERA,
	GEOMETRY,
	TRANSFORMATIONS,
	LIGHTS,
	MATERIALS,

	UNKNOWN_COMMAND

}CommandType;


class SceneParser {

public:

	static RenderInfo* readFile(const char* fileName);

private:


	static set<string> general;
	static string 	   camera;
	static set<string> geometry;
	static set<string> transformations;
	static set<string> lights;
	static set<string> materials;

	static stack<mat4> transformsStack;

	static bool inSet(set<string> &, string&);

	static bool isGeneralCommand(string&);
	static bool isCameraCommand(string&);
	static bool isGeometryCommand(string&);
	static bool isTransformationsCommand(string&);
	static bool isLightsCommand(string&);
	static bool isMaterialsCommand(string&);

	static CommandType identifyCommand(string &);

	static void rightMultiply(const mat4 & M, stack<mat4> &transfstack);
	static bool readValues(std::stringstream &s, const int numValues, GLfloat *values);


	static void handleGeneralCommand(stringstream&, string&);
	static void handleCameraCommand(stringstream&, string&);
	static void handleGeometryCommand(stringstream&, string&);
	static void handleTransformationsCommand(stringstream&, string&);
	static void handleLightsCommand(stringstream&, string&);
	static void handleMaterialsCommand(stringstream&, string&);


	static vec3 ambient;
	static vec3 diffuse;
	static vec3 specular;
	static vec3 emission;
	static GLfloat shininess;
	static GLfloat values[MAX_POSSIBLE_VALUES];

	static Attenuation attenuation;
	static GLuint maxDepth;

	static RenderInfo* renderInfo;
};







#endif //__SCENE_PARSER__
