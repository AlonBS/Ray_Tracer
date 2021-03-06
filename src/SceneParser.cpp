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
#include "Cylinder.h"
#include "Box.h"
#include "Cone.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.hpp"
#include "Plain.h"

using namespace std;


struct Commands {

	// General Scene
	const string size              = "size";
	const string maxdepth          = "maxDepth";
	const string skybox			   = "skybox";

	// Camera
	const string camera            = "camera";

	// Geometry and objects
	const string sphere            = "sphere";
	const string cylinder          = "cylinder";
	const string box		       = "box";
	const string cone		       = "cone";
	const string plain		       = "plain";
	const string maxVerts          = "maxVerts";
	const string vertexSimple      = "vertexSimple";
	const string vertex            = "vertex";
	const string tri               = "tri";
	const string texture           = "texture";
	const string bindTexture       = "bindTexture";
	const string unbindTexture     = "unbindTexture";
	const string envMap            = "envMap";
	const string bindEnvMaps       = "bindEnvMaps";
	const string unbindEnvMaps     = "unbindEnvMaps";
	const string bindNormalMap     = "bindNormalMap";
	const string unbindNormalMap   = "unbindNormalMap";
	const string model			   = "model";


	// Transformations
	const string translate         = "translate";
	const string rotate            = "rotate";
	const string scale             = "scale";
	const string pushTransform     = "pushTransform";
	const string popTransform      = "popTransform";

	// Lights
	const string directional       = "directional";
	const string point             = "point";
	const string area              = "area";
	const string attenuation       = "attenuation";

	// Materials
	const string clearProps        = "clearProps";
	const string ambient           = "ambient"; // As this is per object - and not per scene
	const string diffuse           = "diffuse";
	const string specular          = "specular";
	const string shininess         = "shininess";
	const string emission          = "emission";
	const string reflection        = "reflection";
	const string reflectionBlur    = "reflectionBlur";
	const string refraction        = "refraction";
	const string refractionIndex   = "refractionIndex";
	const string refractionBlur    = "refractionBlur";

}Commands;


/////////////////////////////////////INIT STATIC MEMBERS ///////////////////////////////////////////////////

AdditionalRenderParams SceneParser::additionalParams{};

set<string> SceneParser::general{Commands.size, Commands.maxdepth, Commands.skybox};
string 	    SceneParser::camera = Commands.camera;
set<string> SceneParser::geometry{Commands.sphere, Commands.cylinder, Commands.box, Commands.cone,
								  Commands.plain, Commands.maxVerts,
								  Commands.vertexSimple, Commands.vertex, Commands.tri,
								  Commands.texture, Commands.bindTexture, Commands.unbindTexture,
								  Commands.bindNormalMap, Commands.unbindNormalMap,
								  Commands.envMap, Commands.bindEnvMaps, Commands.unbindEnvMaps,
								  Commands.model};
set<string> SceneParser::transformations {Commands.translate, Commands.rotate, Commands.scale,
										  Commands.pushTransform, Commands.popTransform};
set<string> SceneParser::lights {Commands.directional, Commands.point, Commands.area, Commands.attenuation};
set<string> SceneParser::materials {Commands.clearProps, Commands.ambient, Commands.diffuse, Commands.specular,
									Commands.shininess, Commands.emission,
									Commands.reflection, Commands.reflectionBlur,
									Commands.refraction, Commands.refractionIndex, Commands.refractionBlur};

vector<GLfloat> SceneParser::values = {};


vec3 SceneParser::ambient  = vec3(0.2f, 0.2f, 0.2f);
vec3 SceneParser::diffuse  = vec3(0.0f, 0.0f, 0.0f);
vec3 SceneParser::specular = vec3(0.0f, 0.0f, 0.0f);
vec3 SceneParser::emission = vec3(0.0f, 0.0f, 0.0f);
GLfloat SceneParser::shininess = 0.0f;
vec3 SceneParser::reflection = vec3(0.0f, 0.0f, 0.0f);
GLfloat SceneParser::reflectionBlur = 0.0f;
vec3 SceneParser::refraction = vec3(0.0f, 0.0f, 0.0f);
GLfloat SceneParser::refractionIndex = 0.0f;
GLfloat SceneParser::refractionBlur = 0.0f;



Attenuation SceneParser::attenuation = { .constant = 1.0f, .linear = 0.0f, .quadratic = 0.0f};
GLuint 		SceneParser::maxDepth = RECURSION_DEFAULT_DEPTH;

stack<mat4> SceneParser::transformsStack{};

vector<Vertex> SceneParser::vertices{};

GLint SceneParser::lineNumber = 0;


bool SceneParser::textureIsBound = false;
shared_ptr<const Image> SceneParser::boundTexture = nullptr;
shared_ptr<const Image> SceneParser::boundNormalMap = nullptr;
EnvMaps SceneParser::boundEnvMaps {};

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
SceneParser::readValues(istringstream &s, vector<GLfloat>& values)
{
	values = std::vector<GLfloat> { std::istream_iterator<GLfloat>{s}, std::istream_iterator<GLfloat>{}};
}


void SceneParser::fillObjectInfo(ObjectProperties* op, ObjectTransforms* ot, mat4* uniqueTrans)
{
	if (!op || !ot) return;

	op->_ambient = ambient;
	op->_emission = emission;
	op->_diffuse = diffuse;
	op->_specular = specular;
	op->_shininess = shininess;
	op->_reflection = reflection;
	op->_reflectionBlur = reflectionBlur;
	op->_refraction = refraction;
	op->_refractionIndex = refractionIndex;
	op->_refractionBlur = refractionBlur;

	ot->_transform = (uniqueTrans) ? *uniqueTrans : transformsStack.top();
	ot->_invTransform = inverse(ot->_transform);
	ot->_invTransposeTrans = mat3(transpose(ot->_invTransform));
}


vec3
SceneParser::readColor(istringstream& s)
{
	string c;
	s >> c;
	std::transform(c.begin(), c.end(), c.begin(), ::tolower);
	if (c == "black") {
		return vec3(0.0f, 0.0f, 0.0f);
	}
	if (c == "white") {
		return vec3(1.0f, 1.0f, 1.0f);
	}

	readValues(s, values);

	GLfloat n1 = (GLfloat) strtod(c.c_str(), NULL);
	return normColor(vec3(n1, values[0], values[1]));
}


vec3 SceneParser::normColor(vec3 c)
{
	if (c.x > 1) {
		c.x /= 255.0f;
	}
	if (c.y > 1) {
		c.y /= 255.0f;
	}
	if (c.z > 1) {
		c.z /= 255.0;
	}
	return c;
}


void SceneParser::clearObjectProps()
{
	ambient  = vec3(0.2f, 0.2f, 0.2f);
	diffuse  = vec3(0.0f, 0.0f, 0.0f);
	specular = vec3(0.0f, 0.0f, 0.0f);
	emission = vec3(0.0f, 0.0f, 0.0f);
	shininess = 0.0f;
	reflection = vec3(0.0f, 0.0f, 0.0f);
	reflectionBlur = 0.0f;
	refraction = vec3(0.0f, 0.0f, 0.0f);
	refractionIndex = 0.0f;
	refractionBlur = 0.0f;
}



Scene*
SceneParser::readFile(const AdditionalRenderParams& params, const char* fileName)
{

	string str, cmd;
	ifstream in;


	in.open(fileName);
	if (!in.is_open()) {

		cerr << "Unable to open file for read: " << fileName << endl;
		return nullptr;
	}

	values.reserve(MAX_POSSIBLE_VALUES);
	additionalParams = params;
	scene = new Scene();
	scene->handleAdditionalParams(additionalParams);

	// Default transform
	transformsStack.push(mat4(1.0));
	// Default attenuation
	scene->attenuation() = attenuation;
	scene->maxDepth() = maxDepth;


	while (in) {

		getline(in, str);
		boost::algorithm::trim(str);
		++lineNumber;

		// Ignore comments and white spaces of any kind
		if ( !((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#') ))  {
			continue;
		}

		istringstream s(str);
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

			cout << "\t[E]\tLine: " << lineNumber << " - Unknown command: \"" << cmd << "\". Skipped. " << endl;
			break;
		}
	}

	in.close();

	/* Buffers clean up */
	lineNumber = 0;
	vertices.clear();
	boundTexture = nullptr;
	textureIsBound = false;
	boundNormalMap = nullptr;
	boundEnvMaps.maps.clear();
	boundEnvMaps = {};
	clearObjectProps();
	transformsStack = {};
	attenuation = Attenuation{
		.constant = 1.0f,
		.linear = 0.0f,
		.quadratic = 0.0f
	};
	maxDepth = RECURSION_DEFAULT_DEPTH;

	return scene;
}




void
SceneParser::handleGeneralCommand(istringstream& s, string& cmd)
{
	if (cmd == Commands.size) {

		readValues(s, values);
		scene->width() = values[0];
		scene->height() = values[1];
	} else if (cmd == Commands.maxdepth) {

		readValues(s, values);
		scene->maxDepth() = (GLuint) glm::min(MAX_RECURSION_DEPTH, values[0]);

	} else if (cmd == Commands.skybox) {

		readValues(s, values);
		scene->setSkyBox(values[0]);
	}

}


void
SceneParser::handleCameraCommand(istringstream& s, string& cmd)
{
	(void) cmd; //Suppress warning - maybe we'll have more camera options in the future.
	readValues(s, values);
	vec3 eyeInit = glm::vec3(values[0], values[1], values[2]);
	vec3 center  = glm::vec3(values[3], values[4], values[5]);
	vec3 upInit  = glm::vec3(values[6], values[7], values[8]);
	//upinit = Transform::upvector(upinit, eyeinit);
	GLfloat fovy = values[9];
	scene->addCamera(make_unique<Camera>(eyeInit, center, upInit, fovy, scene->width(), scene->height()));
//	scene->camera() = Camera);
//	transformsStack.top() = lookAt(eyeInit,center,upInit);
}



void
SceneParser::handleGeometryCommand(istringstream& s, string& cmd)
{
	if (cmd == Commands.sphere) {
		readValues(s, values);

		/* We want to allow the user to define a sphere at a specific location in addition to be able to translate it.
		 * To achieve this, we actually create a canonical sphere at (0,0,0) at we derive the translation from the given
		 * coordinates. Note that we want to achieve the T*R*S*(Obj) order of multiplication. So we left multiply here.
		 */
		vec3 center = glm::vec3(0, 0, 0);
		mat4 objectTranslation = glm::translate(mat4(1.0), vec3(values[0], values[1], values[2]));

		// Notice we don't change the top of the stack here, so this won't affect other objects
		mat4 objectTransformation = objectTranslation * transformsStack.top(); // yes - left multiplied!
		GLfloat radius = values[3];

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot, &objectTransformation);

		unique_ptr<Object> sphere = make_unique<Sphere>(op, ot, center, radius);
		if (textureIsBound) {
			sphere->setTextures(boundTexture, &boundNormalMap);
		}
		sphere->computeBoundingBox();
		scene->addObject(sphere);
	}

	else if (cmd == Commands.cylinder) {
		readValues(s, values);
		vec3 center = glm::vec3(0, 0, 0);
		mat4 objectTranslation = glm::translate(mat4(1.0), vec3(values[0], values[1], values[2]));
		mat4 objectTransformation = objectTranslation * transformsStack.top(); // yes - left multiplied! - see note at sphere

		GLfloat height = values[3];
		GLfloat radius = values[4];

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot, &objectTransformation);

		unique_ptr<Object> cylinder = make_unique<Cylinder>(op, ot, center, height, radius);
		if (textureIsBound) {
			cylinder->setTextures(boundTexture, &boundNormalMap);
		}
		cylinder->computeBoundingBox();
		scene->addObject(cylinder);
	}

	else if (cmd == Commands.box) {
		readValues(s, values);
		vec3 minBound = vec3(values[0], values[1], values[2]);
		vec3 maxBound = vec3(values[3], values[4], values[5]);

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot);

		unique_ptr<Object> box = make_unique<Box>(op, ot, minBound, maxBound);
		if (textureIsBound) {
			box->setTextures(boundTexture, &boundNormalMap);
		}
		scene->addObject(box);
	}


	else if (cmd == Commands.cone) {
		readValues(s, values);
		vec3 center = glm::vec3(0, 0, 0);
		mat4 objectTranslation = glm::translate(mat4(1.0), vec3(values[0], values[1], values[2]));
		mat4 objectTransformation = objectTranslation * transformsStack.top(); // yes - left multiplied! - see note at sphere

		GLfloat minCap = values[3];
		GLfloat maxCap = values[4];

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot, &objectTransformation);

		unique_ptr<Object> cone = make_unique<Cone>(op, ot, center, minCap, maxCap);
		if (textureIsBound) {
			cone->setTextures(boundTexture, &boundNormalMap);
		}
		cone->computeBoundingBox();
		scene->addObject(cone);
	}

	else if (cmd == Commands.plain) {

		/* To achieve other plains - use transformations on this object */

		enum TexturePattern tp = MIRRORED_REPEAT;
		if (boundTexture) {
			string texturePattern;
			s >> texturePattern;

			if (texturePattern == "R")
				tp = REPEAT;
			else if (texturePattern == "MR")
				tp = MIRRORED_REPEAT;

			else if (texturePattern == "CE")
				tp = CLAMP_TO_EDGE;
			else {
				cout << "\t[W]\tLine: " << lineNumber << ". No, or invalid texture pattern was given for textured plain. Mirrored-Repeat was chosen. " << endl;
				tp = MIRRORED_REPEAT;
			}
		}

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot);

		unique_ptr<Object> plain = make_unique<Plain>(op, ot, tp);
		if (textureIsBound) {
			plain->setTextures(boundTexture, &boundNormalMap);
		}
		scene->addObject(plain);
	}


	else if (cmd == Commands.maxVerts) {
		// New object is coming
		vertices.clear();
	}

	else if (cmd == Commands.vertexSimple) {
		readValues(s, values);
		vertices.push_back(
				Vertex
				{
					Position : vec3(values[0], values[1], values[2]),
				}
				);
	}

	else if (cmd == Commands.vertex) {
		readValues(s, values);
		vertices.push_back(
				Vertex
				{
					Position   : values.size() > 0 ? vec3(values[0],  values[1],  values[2]) : vec3(0),
					Normal	   : values.size() > 3 ? vec3(values[3],  values[4],  values[5]) : vec3(0),
					TexCoords  : values.size() > 6 ? vec2(values[6],  values[7]) : vec3(0),
					Tangent    : values.size() > 8 ? vec3(values[8],  values[9],  values[10]) : vec3(0),
					Bitangent  : values.size() > 11 ? vec3(values[11], values[12], values[13]) : vec3(0),
				}
				);
	}


	else if (cmd == Commands.tri) {
		readValues(s, values);

		ObjectProperties op{};
		ObjectTransforms ot{};
		fillObjectInfo(&op, &ot);

		Vertex A = vertices[values[0]];
		Vertex B = vertices[values[1]];
		Vertex C = vertices[values[2]];



		A.Position = ot._transform * vec4(A.Position, 1.0f);
		A.Normal = ot._invTransposeTrans * A.Normal;
		A.Tangent = ot._invTransposeTrans * A.Tangent;
		A.Bitangent = ot._invTransposeTrans * A.Bitangent;
		B.Position = ot._transform * vec4(B.Position, 1.0f);
		B.Normal = ot._invTransposeTrans * B.Normal;
		B.Tangent = ot._invTransposeTrans * B.Tangent;
		B.Bitangent = ot._invTransposeTrans * B.Bitangent;
		C.Position = ot._transform * vec4(C.Position, 1.0f);
		C.Normal = ot._invTransposeTrans * C.Normal;
		C.Tangent = ot._invTransposeTrans * C.Tangent;
		C.Bitangent = ot._invTransposeTrans * C.Bitangent;

		unique_ptr<Object> triangle = make_unique<Triangle>(op, A, B, C);
		if (textureIsBound) {
			triangle->setTextures(boundTexture, &boundNormalMap);
		}
		scene->addObject(triangle);

	}


	else if (cmd == Commands.texture) {

		string textureFile;
		s >> textureFile;

		shared_ptr<Image> texture = make_shared<Image>(0, 0);
		texture->loadImage(textureFile);

		scene->addTexture(texture);
	}

	else if (cmd == Commands.bindTexture) {

			readValues(s, values);

			boundTexture = scene->getTexture(values[0]);
			textureIsBound = true;
	}

	else if (cmd == Commands.unbindTexture) {

		textureIsBound = false;
		boundTexture = nullptr;
	}

	else if (cmd == Commands.bindNormalMap) {

			readValues(s, values);
			boundNormalMap = scene->getTexture(values[0]);
	}

	else if (cmd == Commands.unbindNormalMap) {

		boundNormalMap = nullptr;
	}

	else if (cmd == Commands.envMap) {

		string envMapFile;
		s >> envMapFile;

		shared_ptr<Image> envMap = make_shared<Image>(0, 0);
		envMap->loadImage(envMapFile);
		scene->addEnvMap(envMap);
	}

	else if (cmd == Commands.bindEnvMaps) {

		// TODO - FIX PROBLEM HERE using new readvalues.
		readValues(s, values);
		string envMapType;
		s >> envMapType;

		boundEnvMaps.maps = scene->getEnvMapsImages(values[0]);
		boundEnvMaps.refractiveMapping = (envMapType == "refract");
		if (boundEnvMaps.refractiveMapping) {
			readValues(s, values);
			boundEnvMaps.refractiveIndex = values[0];
		}

	}

	else if (cmd == Commands.unbindEnvMaps) {

		boundEnvMaps.maps.clear();
	}
	else if (cmd == Commands.model) {
		string modelFile;
		s >> modelFile;

		ObjectProperties op;
		ObjectTransforms ot;
		fillObjectInfo(&op, &ot);

		vector<shared_ptr<const Mesh>> modelMeshes{};
		Model::loadModel(
				modelFile,
				op,
				ot,
				boundTexture,
				boundNormalMap,
				boundEnvMaps,
				modelMeshes);
		scene->addMeshes(modelMeshes);
	}
}


void
SceneParser::handleTransformationsCommand(istringstream& s, string& cmd)
{
	if (cmd == Commands.translate) {
		readValues(s, values);
		transformsStack.top() = glm::translate(transformsStack.top(), vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.scale) {
		readValues(s, values);
		transformsStack.top() = glm::scale(transformsStack.top(), vec3(values[0], values[1], values[2]));
	}

	else if (cmd == Commands.rotate) {
		readValues(s, values);
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
SceneParser::handleLightsCommand(istringstream& s, string& cmd)
{
	if (cmd == Commands.directional) {

		readValues(s, values);
		vec3 dir = vec3(values[0], values[1], values[2]);
		vec3 color = normColor(vec3(values[3], values[4], values[5]));
		GLfloat intensity = values[6];
		unique_ptr<const DirectionalLight> dirLight = make_unique<const DirectionalLight>(color, intensity, dir, transformsStack.top());

		scene->addDirectionalLight(dirLight);
	}

	else if (cmd == Commands.point) {
		readValues(s, values);

		vec3 pos = vec3(0, 0, 0);
		mat4 lightTranslation = glm::translate(mat4(1.0), vec3(values[0], values[1], values[2]));
		mat4 lightTransformation = lightTranslation * transformsStack.top(); // yes - left multiplied! - see note at sphere
		vec3 color = normColor(vec3(values[3], values[4], values[5]));
		GLfloat intensity = values[6];
		unique_ptr<const PointLight> pointLight = make_unique<const PointLight>(color, intensity, pos, lightTransformation);

		scene->addPointLight(pointLight);
	}

	else if (cmd == Commands.area) {
		readValues(s, values);

		vec3 center = vec3(0, 0, 0);
		mat4 lightTranslation = glm::translate(mat4(1.0), vec3(values[0], values[1], values[2]));
		mat4 lightTransformation = lightTranslation * transformsStack.top(); // yes - left multiplied! - see note at sphere
		vec3 color = normColor(vec3(values[3], values[4], values[5]));
		GLfloat intensity = values[6];
		GLfloat radius = values[7];

		if (additionalParams.hardShadows) {
			unique_ptr<const PointLight> pointLight = make_unique<const PointLight>(color, intensity, center, lightTransformation);
			scene->addPointLight(pointLight);
		}
		else {

			unique_ptr<const AreaLight> areaLight = make_unique<const AreaLight>(color, intensity, center, radius, lightTransformation, 16);
			scene->addAreaLight(areaLight);
		}

	}

	else if (cmd == Commands.attenuation) {
		readValues(s, values);
		Attenuation atten = {
				.constant = values[0],
				.linear = values[1],
				.quadratic = values[2]
		};

		scene->attenuation() = atten;
	}
}





void
SceneParser::handleMaterialsCommand(istringstream& s, string& cmd)
{
	if (cmd == Commands.clearProps) {
		clearObjectProps();
	}

	if (cmd == Commands.ambient) {

		ambient = readColor(s);
	}

	else if (cmd == Commands.diffuse) {

		diffuse = readColor(s);
	}

	else if (cmd == Commands.specular) {

		specular = readColor(s);
	}

	else if (cmd == Commands.emission) {

		emission = readColor(s);
	}

	else if (cmd == Commands.shininess) {
		readValues(s, values);
		shininess = values[0];
	}

	else if (cmd == Commands.reflection) {

		reflection = readColor(s);
	}

	else if (cmd == Commands.reflectionBlur) {

		readValues(s, values);
		reflectionBlur = values[0];
	}

	else if (cmd == Commands.refraction) {

		refraction = readColor(s);
	}

	else if (cmd == Commands.refractionIndex) {
		readValues(s, values);
		refractionIndex = values[0];
	}

	else if (cmd == Commands.refractionBlur) {
		readValues(s, values);
		refractionBlur = values[0];
	}

}


