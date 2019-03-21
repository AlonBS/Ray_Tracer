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
    /*  Model Data */
//    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

//    vector<Mesh*> meshes;
//    string directory;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.

    /**
     * TODO - add doc
     */
    static void loadModel(string const &path, const ObjectProperties& op, const ObjectTransforms& ot, Image* texture,
    					  vector<Mesh*>& modelMeshes, vector<Image*>& modelTextures);

    static void FreeTextures();

private:

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    static void processNode(aiNode *node, const aiScene *scene);
    static Mesh* processMesh(aiMesh *mesh, const aiScene *scene);

    struct Texture {

    	Image *texture;
    	string name;
    };

    static vector<Mesh*> _meshes;
    static ObjectProperties _objectProperties;
    static ObjectTransforms _objectTransforms;
    static Image* _texture;

    static Image* loadMaterialTextures(aiMaterial* mat, aiTextureType type);
    static vector<Texture*> _loadedTextures; // We store all the textures loaded for this module, to avoid load duplication
    static string _directory;
};


#endif
