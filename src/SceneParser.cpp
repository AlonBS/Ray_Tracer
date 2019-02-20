/*****************************************************************************/

/*****************************************************************************/

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <GL/glew.h>
#include <GL/glut.h>
#include <boost/algorithm/string.hpp>

#include "SceneParser.h"
#include "Object.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Model.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.hpp"

using namespace std;

#define RECURSION_DEFAULT_DEPTH 2




struct Commands {

	// General Scene
	const string size          = "size";
	const string maxdepth      = "maxDepth";

	// Camera
	const string camera        = "camera";

	// Geometry and objects
	const string sphere        = "sphere";
	const string maxVerts      = "maxVerts";
	const string maxVertNorms  = "maxVertNorms";
	const string vertex        = "vertex";
	const string vertexNormal  = "vertexNormal";
	const string vertexTex     = "vertexTex";
	const string tri           = "tri";
	const string triNormal     = "triNormal";
	const string triTex        = "triTex";
	const string texture       = "texture";
	const string bindTexture   = "bindTexture";
	const string unbindTexture = "unbindTexture";
	const string model		   = "model";

	// Transformations
	const string translate     = "translate";
	const string rotate        = "rotate";
	const string scale         = "scale";
	const string pushTransform = "pushTransform";
	const string popTransform  = "popTransform";

	// Lights
	const string directional   = "directional";
	const string point         = "point";
	const string attenuation   = "attenuation";

	// Materials
	const string ambient       = "ambient"; // As this is per object - and not per scene
	const string diffuse       = "diffuse";
	const string specular      = "specular";
	const string shininess     = "shininess";
	const string emission      = "emission";

}Commands;


/////////////////////////////////////INIT STATIC MEMBERS ///////////////////////////////////////////////////

set<string> SceneParser::general{Commands.size, Commands.maxdepth};
string 	    SceneParser::camera = Commands.camera;
set<string> SceneParser::geometry{Commands.sphere, Commands.maxVerts, Commands.maxVertNorms,
								  Commands.vertex, Commands.vertexNormal, Commands.vertexTex, Commands.tri,
								  Commands.triNormal, Commands.triTex, Commands.texture, Commands.bindTexture, Commands.unbindTexture,
								  Commands.model};
set<string> SceneParser::transformations {Commands.translate, Commands.rotate, Commands.scale,
										  Commands.pushTransform, Commands.popTransform};
set<string> SceneParser::lights {Commands.directional, Commands.point, Commands.attenuation};
set<string> SceneParser::materials {Commands.ambient, Commands.diffuse, Commands.specular, Commands.shininess, Commands.emission};


vec3 SceneParser::ambient  = vec3(0.2f, 0.2f, 0.2f);
vec3 SceneParser::diffuse  = vec3(0.0f, 0.0f, 0.0f);
vec3 SceneParser::specular = vec3(0.0f, 0.0f, 0.0f);
vec3 SceneParser::emission = vec3(0.0f, 0.0f, 0.0f);
GLfloat SceneParser::shininess = 0.0f;
GLfloat SceneParser::values[MAX_POSSIBLE_VALUES] = {};

Attenuation SceneParser::attenuation = { .constant = 1.0f, .linear = 0.0f, .quadratic = 0.0f};
GLuint 		SceneParser::maxDepth = RECURSION_DEFAULT_DEPTH;

stack<mat4> SceneParser::transformsStack{};

vector<glm::vec3> SceneParser::vertices{};
vector<glm::vec3> SceneParser::verticesNormals;

vector<glm::vec3> SceneParser::verticesTexV{};
vector<glm::vec2> SceneParser::verticesTexT{};

Image* SceneParser::boundTexture = nullptr;
bool SceneParser::textureIsBound = false;
Scene* SceneParser::scene = nullptr;


///////////////////////////////////////////////////////////////////////////////////////////////////////////


bool
SceneParser::inSet(set<string>& set, string& cmd)
{
	return set.find(cmd) != set.end();
}
bool
SceneParser::isGeneralCommand(string& cmd)
{
	return inSet(general, cmd);
}

bool
SceneParser::isCameraCommand(string& cmd)
{
	return cmd == SceneParser::camera;
}
bool
SceneParser::isGeometryCommand(string& cmd)
{
	return inSet(geometry, cmd);
}
bool
SceneParser::isTransformationsCommand(string& cmd)
{
	return inSet(transformations, cmd);
}
bool
SceneParser::isLightsCommand(string& cmd)
{
	return inSet(lights, cmd);
}
bool
SceneParser::isMaterialsCommand(string& cmd)
{
	return inSet(materials, cmd);
}

CommandType
SceneParser::identifyCommand(string & cmd)
{
	if (isGeneralCommand(cmd)) {
		return GENERAL;
	}
	else if (isCameraCommand(cmd)) {
		return CAMERA;
	}
	else if (isGeometryCommand(cmd)) {
		return GEOMETRY;
	}
	else if (isTransformationsCommand(cmd)) {
		return TRANSFORMATIONS;
	}
	else if (isLightsCommand(cmd)) {
		return LIGHTS;
	}
	else if (isMaterialsCommand(cmd)) {
		return MATERIALS;
	}
	return UNKNOWN_COMMAND;
}

void
rightMultiply(const mat4 & M, stack<mat4> &transfstack)
{
	mat4 &T = transfstack.top();
	T = T * M;

}

bool
SceneParser::readValues(stringstream &s, const int numOfVals, GLfloat* values)
{
	for (int i = 0; i < numOfVals; ++i) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}


Scene*
SceneParser::readFile(const char* fileName)
{
	string str, cmd;
	ifstream in;



	in.open(fileName);
	if (!in.is_open()) {

		cerr << "Unable to open file for read: " << fileName << endl;
		return nullptr;
	}

	scene = new Scene();

	// Default transform
	transformsStack.push(mat4(1.0));
	// Default attenuation
	scene->attenuation() = attenuation;
	scene->maxDepth() = maxDepth;

	while (in) {

		getline(in, str);
		boost::algorithm::trim(str);

		// Ignore comments and white spaces of any kind
		if ( !((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#') ))  {
			continue;
		}

		stringstream s(str);
		s >> cmd;

		CommandType command = SceneParser::identifyCommand(cmd);
		switch (command) {
		case GENERAL:
			handleGeneralCommand(s, cmd);
			break;

		case CAMERA:
			handleCameraCommand(s, cmd);
			break;

		case GEOMETRY:
			handleGeometryCommand(s, cmd);
			break;

		case TRANSFORMATIONS:
			handleTransformationsCommand(s, cmd);
			break;

		case LIGHTS:
			handleLightsCommand(s, cmd);
			break;

		case MATERIALS:
			handleMaterialsCommand(s, cmd);
			break;

		case UNKNOWN_COMMAND:
		default:

			cout << "\nUnknown command: " << cmd << ". Skipped. " << endl;
			break;
		}

	}

	in.close();

	/* BUffers clean up */
	vertices.clear();
	verticesNormals.clear();
	verticesTexV.clear();
	verticesTexT.clear();

	return scene;
}




void
SceneParser::handleGeneralCommand(stringstream& s, string& cmd)
{
	if (cmd == Commands.size) {

		readValues(s, 2, values);
		scene->width() = values[0];
		scene->height() = values[1];
	} else if (cmd == Commands.maxdepth) {

		readValues(s, 1, values);
		scene->maxDepth() = values[0];

	}

}


void
SceneParser::handleCameraCommand(stringstream& s, string& cmd)
{
	(void) cmd; //Suppress warning - maybe we'll have more camera options in the future.
	readValues(s, 10, values);
	vec3 eyeInit = glm::vec3(values[0], values[1], values[2]);
	vec3 center  = glm::vec3(values[3], values[4], values[5]);
	vec3 upInit  = glm::vec3(values[6], values[7], values[8]);
	//upinit = Transform::upvector(upinit, eyeinit);
	GLfloat fovy = values[9];
	scene->camera() = Camera(eyeInit, center, upInit, fovy, scene->width(), scene->height());
	//transformsStack.top() = lookAt(eyeInit,center,upInit);
}


void
SceneParser::handleGeometryCommand(stringstream& s, string& cmd)
{
	if (cmd == Commands.sphere) {
		readValues(s, 4, values);
		vec3 center = glm::vec3(values[0], values[1], values[2]);
		GLfloat radius = values[3];
		Object *sphere = new Sphere(center, radius);
		sphere->ambient() = ambient;
		sphere->emission() = emission;
		sphere->diffuse() = diffuse;
		sphere->specular() = specular;
		sphere->shininess() = shininess;
		sphere->transform() = transformsStack.top();
		sphere->invTransform() = inverse(sphere->transform());
		sphere->invTransposeTrans() = mat3(transpose(sphere->invTransform()));

		if (textureIsBound) {
			sphere->setTexture(boundTexture);
		}
		scene->addObject(sphere);
	}

	else if (cmd == Commands.maxVerts) {
		// New object is coming
		vertices.clear();
	}

	else if (cmd == Commands.maxVertNorms) {
		// New Object is coming
		verticesNormals.clear();
	}

	else if (cmd == Commands.vertex) {
		readValues(s, 3, values);
		vertices.push_back(vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.vertexNormal) {
		readValues(s, 6, values);
		verticesNormals.push_back(vec3(values[0], values[1], values[2]));
		verticesNormals.push_back(vec3(values[3], values[4], values[5]));
	}

	else if (cmd == Commands.vertexTex) {
		readValues(s, 5, values);
		verticesTexV.push_back(vec3(values[0], values[1], values[2]));
		verticesTexT.push_back(vec2(values[3], values[4]));
	}


	else if (cmd == Commands.tri) {
		readValues(s, 3, values);
		vec3 A = vec3 (transformsStack.top() * vec4(vertices[values[0]], 1.0f));
		vec3 B = vec3 (transformsStack.top() * vec4(vertices[values[1]], 1.0f));
		vec3 C = vec3 (transformsStack.top() * vec4(vertices[values[2]], 1.0f));
		Object *triangle = new Triangle(A, B, C);

//		Object *triangle = new Triangle(renderInfo.vertcies[values[0]],
//												renderInfo.vertcies[values[1]],
//												renderInfo.vertcies[values[2]]);
		triangle->ambient() = ambient;
		triangle->emission() = emission;
		triangle->diffuse() = diffuse;
		triangle->specular() = specular;
		triangle->shininess() = shininess;

//		triangle->transform() = transformsStack.top();
//		triangle->invTransform() = inverse(triangle->transformMat);
//		triangle->invTransposeTrans() = transpose(triangle->invTransformMat);
		if (textureIsBound) {
			triangle->setTexture(boundTexture);
		}
		scene->addObject(triangle);
	}

	else if (cmd == Commands.triNormal) {
		readValues(s, 3, values);
		// TODO - need to transform normals too ?
		//TODO - transform !! )no
		Object *triangle = new Triangle(vertices[values[0] * 2],
										vertices[values[1] * 2],
										vertices[values[2] * 2],
										vertices[values[0] * 2 - 1],
										vertices[values[1] * 2 - 1],
										vertices[values[2] * 2 - 1]);

		if (textureIsBound) {
			triangle->setTexture(boundTexture);
		}
		scene->addObject(triangle);
	}

	else if (cmd == Commands.triTex) {
		readValues(s, 3, values);
		vec3 A = vec3 (transformsStack.top() * vec4(verticesTexV[values[0]], 1.0f));
		vec3 B = vec3 (transformsStack.top() * vec4(verticesTexV[values[1]], 1.0f));
		vec3 C = vec3 (transformsStack.top() * vec4(verticesTexV[values[2]], 1.0f));
		vec2 Auv = vec2(verticesTexT[values[0]]);
		vec2 Buv = vec2(verticesTexT[values[1]]);
		vec2 Cuv = vec2(verticesTexT[values[2]]);

		Object *triangle = new Triangle(A, B, C, Auv, Buv, Cuv);

		//		Object *triangle = new Triangle(renderInfo.vertcies[values[0]],
		//												renderInfo.vertcies[values[1]],
		//												renderInfo.vertcies[values[2]]);
		triangle->ambient() = ambient;
		triangle->emission() = emission;
		triangle->diffuse() = diffuse;
		triangle->specular() = specular;
		triangle->shininess() = shininess;

		//		triangle->transform() = transformsStack.top();
		//		triangle->invTransform() = inverse(triangle->transformMat);
		//		triangle->invTransposeTrans() = transpose(triangle->invTransformMat);
		if (textureIsBound) {
			triangle->setTexture(boundTexture);
		}
		scene->addObject(triangle);
	}

	else if (cmd == Commands.texture) {

		string textureFile;
		s >> textureFile;

		Image *texture = new Image(0, 0);
		texture->loadImage(textureFile);

		scene->addTexture(texture);
	}

	else if (cmd == Commands.bindTexture) {

			readValues(s, 1, values);

			boundTexture = scene->getTexture(values[0]);
			textureIsBound = true;
	}

	else if (cmd == Commands.unbindTexture) {

			textureIsBound = false;
		}

	else if (cmd == Commands.model) {
		string modelFile;
		s >> modelFile;

		mat4 transform = transformsStack.top();
		mat3 normalsTransform = mat3(transpose(inverse(transform)));
		Object *model = new Model(modelFile, transform, normalsTransform);
		model->ambient() = ambient;
		model->emission() = emission;
		model->diffuse() = diffuse;
		model->specular() = specular;
		model->shininess() = shininess;
		scene->addObject(model);
	}
}


void
SceneParser::handleTransformationsCommand(stringstream& s, string& cmd)
{
	if (cmd == Commands.translate) {
		readValues(s,3,values);
		transformsStack.top() = glm::translate(transformsStack.top(), vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.scale) {
		readValues(s,3,values);
		transformsStack.top() = glm::scale(transformsStack.top(), vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.rotate) {
		readValues(s,4,values);
		transformsStack.top() = glm::rotate(transformsStack.top(), radians(values[3]), vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.pushTransform) {
		transformsStack.push(transformsStack.top());
	}

	else if (cmd == Commands.popTransform) {
		if (transformsStack.size() <= 1) {
			cerr << "Stack has no elements.  Cannot Pop\n";
		} else {
			transformsStack.pop();
		}
	}
}


void
SceneParser::handleLightsCommand(stringstream& s, string& cmd)
{
	if (cmd == Commands.directional) {
		readValues(s, 6, values);
		vec3 dir = vec3(transformsStack.top() * vec4(values[0], values[1], values[2], 0.0f));
		vec3 color = vec3(values[3], values[4], values[5]);
		DirectionalLight *dirLight = new DirectionalLight(color, dir);

		scene->addDirectionalLight(dirLight);
	}

	else if (cmd == Commands.point) {
		readValues(s, 6, values);
		//TODO CHECK
		vec3 pos = vec3(transformsStack.top() * vec4(values[0], values[1], values[2], 1.0f));
		vec3 color = vec3(values[3], values[4], values[5]);
		PointLight *pointLight = new PointLight(color, pos);
		scene->addPointLight(pointLight);
	}

	else if (cmd == Commands.attenuation) {
		readValues(s, 3, values);
		Attenuation atten = {
				.constant = values[0],
				.linear = values[1],
				.quadratic = values[2]
		};

		scene->attenuation() = atten;
	}

}


void
SceneParser::handleMaterialsCommand(stringstream& s, string& cmd)
{
	if (cmd == Commands.ambient) {
		readValues(s, 3, values);
		ambient = vec3(values[0], values[1], values[2]);
	}

	else if (cmd == Commands.diffuse) {
		readValues(s, 3, values);
		diffuse = vec3(values[0], values[1], values[2]);

	}

	else if (cmd == Commands.specular) {
		readValues(s, 3, values);
		specular = vec3(values[0], values[1], values[2]);
	}

	else if (cmd == Commands.emission) {
		readValues(s, 3, values);
		emission = vec3(values[0], values[1], values[2]);
	}

	else if (cmd == Commands.shininess) {
		readValues(s, 1, values);
		shininess = values[0];
	}

}


