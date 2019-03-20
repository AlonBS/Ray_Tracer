#ifndef MODEL_H
#define MODEL_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Object.h"



using namespace std;

//unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model : public Object
{
public:
    /*  Model Data */
//    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh*> meshes;
//    string directory;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model();

    virtual ~Model();

    // TODO - currently this won't be in use - refactor code
    virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) = 0;


    /**
     * TODO - add doc
     */
    void loadModel(string const &path);

private:

    /*  Functions   */

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);

    Mesh* processMesh(aiMesh *mesh, const aiScene *scene);



    struct Texture {

    	Image *texture;
    	string name;
    };

    Image* loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    vector<Texture*> loadedTextures; // We store all the textures loaded for this module, to avoid load duplication

    string directory;
};


#endif
