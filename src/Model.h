#ifndef MODEL_H
#define MODEL_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace std;


class Model
{
public:

    /**
     * TODO - add doc
     */
    static void loadModel(string const &path,
    					  const ObjectProperties& op,
						  const ObjectTransforms& ot,
						  shared_ptr<const Image> texture,
						  shared_ptr<const Image> global_normalMap,
						  EnvMaps& envMaps,
    					  vector<shared_ptr<const Mesh>>& modelMeshes);

//    static void FreeTextures();

private:

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    static void processNode(aiNode *node, const aiScene *scene);
    static shared_ptr<const Mesh> processMesh(aiMesh *mesh, const aiScene *scene);

    struct Texture {

    	std::shared_ptr<Image> texture;
    	string name;
    };

    static vector<shared_ptr<const Mesh>> _meshes;
    static ObjectProperties _objectProperties;
    static ObjectTransforms _objectTransforms;
    static shared_ptr<const Image> _globalTexture;
    static shared_ptr<const Image> _globalNormalMap;
    static EnvMaps _envMaps;




    static std::shared_ptr<Image> loadMaterialTextures(aiMaterial* mat, aiTextureType type);
    static vector<Texture> _loadedTextures; // We store all the textures loaded for this module, to avoid load duplication
    static string _directory;
};


#endif
