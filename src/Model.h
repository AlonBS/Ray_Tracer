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
    static vector<Mesh*> loadModel(string const &path, ObjectProperties& op, ObjectTransforms& ot);

private:

    static vector<Mesh*> meshes;
    static ObjectProperties objectProperties;
    static ObjectTransforms objectTransforms;

    /*  Functions   */

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    static void processNode(aiNode *node, const aiScene *scene);

    static Mesh* processMesh(aiMesh *mesh, const aiScene *scene);



    struct Texture {

    	Image *texture;
    	string name;
    };

    static Image* loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    static vector<Texture*> loadedTextures; // We store all the textures loaded for this module, to avoid load duplication

    static string directory;
};


#endif
