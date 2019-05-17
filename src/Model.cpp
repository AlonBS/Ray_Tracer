

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Model.h"

using namespace std;




vector<shared_ptr<const Mesh>> Model::_meshes {};
ObjectProperties Model::_objectProperties{};
ObjectTransforms Model::_objectTransforms{};
shared_ptr<const Image> Model::_globalTexture = nullptr;
shared_ptr<const Image> Model::_globalNormalMap = nullptr;
EnvMaps Model::_envMaps = {};
vector<Model::Texture> Model::_loadedTextures{}; // We store all the textures loaded for this module, to avoid load duplication
string Model::_directory{};



void
Model::loadModel(string const &path,
				 const ObjectProperties& op,
				 const ObjectTransforms& ot,
				 shared_ptr<const Image> globalTexture,
				 shared_ptr<const Image> globalNormalMap,
				 EnvMaps& envMaps,
				 OUT vector<shared_ptr<const Mesh>>& modelMeshes)

{
	_objectProperties = op;
	_objectTransforms = ot;
	_globalTexture = globalTexture;
	_globalNormalMap = globalNormalMap;
	_envMaps = envMaps;


	// read file via ASSIMP
	Assimp::Importer importer;
	//const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	uint importFlags = aiProcess_ValidateDataStructure |
					   aiProcess_Triangulate |
					   aiProcess_JoinIdenticalVertices |
					   aiProcess_RemoveRedundantMaterials |
					   aiProcess_FixInfacingNormals |
					   aiProcess_SortByPType |
					   aiProcess_OptimizeMeshes |
					   aiProcess_CalcTangentSpace;

	//aiProcess_FlipUVs


	const aiScene* scene = importer.ReadFile(path, importFlags);
	// check for errors
	if(!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;

		return;
	}

	// retrieve the directory path of the filepath
	_directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	processNode(scene->mRootNode, scene);

	modelMeshes = _meshes;

	_meshes.clear();
	_loadedTextures.clear();
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void
Model::processNode(aiNode *node, const aiScene *scene)
{
	// process each mesh located at the current node
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// the node object only contains indices to index the actual objects in the scene.
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(processMesh(mesh, scene));
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

}


shared_ptr<const Mesh>
Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
	// data to fill
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	MeshProperties properties = {};
	std::shared_ptr<Image> ambientTexture, diffuseTexture, specularTexture, normalsMap;
	ambientTexture = diffuseTexture = specularTexture = normalsMap = nullptr;

	properties._ambient = vec3(1.0f, 1.0f, 1.0f);

	// Walk through each of the mesh's vertices
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex{};
		glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		// positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vec3 (_objectTransforms._transform * vec4(vector, 1.0f));;
		// normals
		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = normalize(_objectTransforms._invTransposeTrans * vector);
		}
		else {
			vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		// texture coordinates
		if(mesh->HasTextureCoords(0)) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		if (mesh->HasTangentsAndBitangents()) {

			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = normalize(_objectTransforms._invTransposeTrans * vector);
			// bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = normalize(_objectTransforms._invTransposeTrans * vector);
		}

		vertices.push_back(vertex);
	}
	// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for(unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for(unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}


	// Process materials
	if(mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		aiColor3D c (0.0f, 0.0f, 0.0f);
		material->Get(AI_MATKEY_COLOR_AMBIENT, c);
		properties._ambient = vec3(c.r, c.g, c.b);
		if (equalToVec3(properties._ambient, COLOR_BLACK))
			properties._ambient = COLOR_WHITE;
		material->Get(AI_MATKEY_COLOR_DIFFUSE, c);
		properties._diffuse = vec3(c.r, c.g, c.b);
		if (equalToVec3(properties._diffuse, COLOR_BLACK))
			properties._diffuse = COLOR_WHITE;
		material->Get(AI_MATKEY_COLOR_SPECULAR, c);
		properties._specular = vec3(c.r, c.g, c.b);
		if (equalToVec3(properties._specular, COLOR_BLACK))
			properties._specular = COLOR_WHITE;
		material->Get(AI_MATKEY_SHININESS, properties._shininess);
		if (properties._shininess == 0.0f)
			properties._shininess = 2;


		// Load textures
		// Currently - we only support one ambient, one diffuse, and one specular texture, as well as normals map
		// 1. Ambient maps
		ambientTexture = loadMaterialTextures(material, aiTextureType_AMBIENT);
		// 2. Diffuse maps
		diffuseTexture = loadMaterialTextures(material, aiTextureType_DIFFUSE);
		// 3. Specular maps
		specularTexture = loadMaterialTextures(material, aiTextureType_SPECULAR);
		// 4. Normals Map
		normalsMap = loadMaterialTextures(material, aiTextureType_HEIGHT);
	}


	// return a mesh object created from the extracted mesh data
	ObjectProperties op = _objectProperties * properties;
	MeshTextures mt = {
			ambientTexture  : ambientTexture,
			diffuseTexture  : diffuseTexture,
			specularTexture : specularTexture,
			normalMap      : normalsMap,
			globalTexture   : _globalTexture,
			globalNormalMap : _globalNormalMap,
			envMaps         : _envMaps
	};
	//return new Mesh(vertices, indices, op, ambientTexture, diffuseTexture, specularTexture, _texture, _envMaps);
	return make_shared<const Mesh>(vertices, indices, op, mt);
}



std::shared_ptr<Image>
Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
	if (mat->GetTextureCount(type) == 0) {
		return nullptr;
	}

	std::shared_ptr<Image> texture = nullptr;
	aiString str;

	mat->GetTexture(type, 0, &str); // Currenly - we support only 1 texture of each type.

	// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
	GLboolean skip = false;
	for(GLuint j = 0; j < _loadedTextures.size(); ++j)
	{
		if(_loadedTextures[j].name.compare(str.C_Str()) == 0)
		{
			texture = _loadedTextures[j].texture;
			skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
			break;
		}
	}
	if(!skip)
	{   // If texture hasn't been loaded already, load it
		Texture loadedTexture = Texture();
		loadedTexture.name = string(str.C_Str());
		loadedTexture.texture = std::make_shared<Image>(0, 0);
		string path = _directory + "/" + loadedTexture.name;
		loadedTexture.texture->loadImage(path);

		texture = loadedTexture.texture;
		_loadedTextures.push_back(loadedTexture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
	}

	return texture;
}


