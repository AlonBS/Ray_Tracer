#ifndef MESH_H
#define MESH_H


#include <glm/glm.hpp>
#include <vector>

#include "Triangle.h"
#include "Box.h"


using namespace std;


typedef struct MeshProperties : ObjectProperties {
	/* Currently doesn't have extra features */

	friend ObjectProperties operator*(const MeshProperties& mp, const ObjectProperties& op);
	friend ObjectProperties operator*(const ObjectProperties& op, const MeshProperties& mp);

}MeshProperties;


class Mesh{

private:

	MeshProperties _properties;
	vector<Triangle*> triangles;

	vector<Vertex> _vertices;

	//	Box* boundingBox; TODO - remove
	//	BoundingVolume* boundingVolume; TODO - remove

	glm::vec3 ambient;
	glm::vec3 emmision;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;

	Image *_ambientTexture;
	Image *_diffuseTexture;
	Image *_specularTexture;


	vec3 __getTextureColor(Image* texture, vec2& uv);


public:


	Mesh(vector<Vertex>& vertices,
			vector<unsigned int>& indices,
			MeshProperties& properties,
			Image *ambientTexture,
			Image *diffuseTexture,
			Image *specualrTexture);


	virtual ~Mesh();


	bool intersectsRay(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, vec2* texCoords, MeshProperties* properties);


	vec3 getAmbientTextureColor(vec2& uv);
	vec3 getDiffuseTextureColor(vec2& uv);
	vec3 getSpecularTextureColor(vec2& uv);

	vector<Vertex>& getVertices() { return _vertices; }


private:


	void __triangulate(vector<Vertex> vertices, vector<unsigned int> indices);

	//    void __computeBoundingBox(vector<Vertex>& vertices);
	//    void __computeBoundingVolume(vector<Vertex>& vertices);

};
#endif
