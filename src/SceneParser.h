// Readfile definitions


#ifndef __SCENE_PARSER__
#define __SCENE_PARSER__

#include <GL/glew.h>


#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <stack>
#include "Camera.hpp"
#include "Scene.h"

using namespace std;

#define MAX_POSSIBLE_VALUES 14

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

	static Scene* readFile(const AdditionalRenderParams& params, const char* fileName);

private:

	static AdditionalRenderParams additionalParams;

	static GLint lineNumber;


	static set<string> general;
	static string 	   camera;
	static set<string> geometry;
	static set<string> transformations;
	static set<string> lights;
	static set<string> materials;


	static bool inSet(set<string> &, string&);

	static bool isGeneralCommand(string&);
	static bool isCameraCommand(string&);
	static bool isGeometryCommand(string&);
	static bool isTransformationsCommand(string&);
	static bool isLightsCommand(string&);
	static bool isMaterialsCommand(string&);

	static CommandType identifyCommand(string &);


	static void rightMultiply(const mat4 & M, stack<mat4> &transfstack);
	static bool readValues(std::stringstream &s, vector<GLfloat>& values);
	static void fillObjectInfo(ObjectProperties* op, ObjectTransforms* ot, mat4* uniqueTrans=nullptr);

	static vec3 readColor(stringstream& s);
	static vec3 normColor(vec3 c);
	static void clearObjectProps();

	static void handleGeneralCommand(stringstream&, string&);
	static void handleCameraCommand(stringstream&, string&);
	static void handleGeometryCommand(stringstream&, string&);
	static void handleTransformationsCommand(stringstream&, string&);
	static void handleLightsCommand(stringstream&, string&);
	static void handleMaterialsCommand(stringstream&, string&);





	/*
	 *  These values serve as buffer for commands that are fully determined
	 *   when an object is created.
	 */

	static vector<GLfloat> values;
	//static GLfloat values[MAX_POSSIBLE_VALUES];
	static vec3 ambient;
	static vec3 diffuse;
	static vec3 specular;
	static vec3 emission;
	static GLfloat shininess;
	static vec3 reflection;
	static GLfloat reflectionBlur;
	static vec3 refraction;
	static GLfloat refractionIndex;
	static GLfloat refractionBlur;

	static Attenuation attenuation;
	static GLuint maxDepth;

	static stack<mat4> transformsStack;

	static vector<Vertex> vertices;
//	static vector<glm::vec3> verticesNormals;
//
//	static vector<glm::vec3> verticesTexV; // For Vertices with textures mapping
//	static vector<glm::vec2> verticesTexT; // The texture mapping
//
//	static vector<glm::vec3> verticesNormTexV; // For vertices with normals and texture mapping
//	static vector<glm::vec3> verticesNormTexN;
//	static vector<glm::vec2> verticesNormTexT;

	static shared_ptr<const Image> boundTexture;
	static bool textureIsBound;
//	static const Image* boundNormalMap;
	static EnvMaps boundEnvMaps;

	/***********************************************************************/



	static Scene* scene;
};







#endif //__SCENE_PARSER__
