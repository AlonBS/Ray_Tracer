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



class Mesh : Object {

private:

	using super = Object;

	vector<Triangle*> triangles;

	vector<Vertex> _vertices;

	//	Box* boundingBox; TODO - remove
	//	BoundingVolume* boundingVolume; TODO - remove

//	glm::vec3 ambient;
//	glm::vec3 emmision;
//	glm::vec3 diffuse;
//	glm::vec3 specular;
//	GLfloat shininess;

	Image* _meshAmbientTexture;
	Image* _meshDiffuseTexture;
	Image* _meshSpecularTexture;

	//const Model* const _model; - TODO


public:


	Mesh(vector<Vertex>& vertices,
			vector<unsigned int>& indices,
			ObjectProperties& properties,
			Image *ambientTexture,
			Image *diffuseTexture,
			Image *specualrTexture,
			Image *generalTexture

			/*const Model* const* - TODO */);


	virtual ~Mesh();


//	bool intersectsRay(const Ray& r, GLfloat* dist, vec3* point, vec3* normal, vec2* texCoords, MeshProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const;


	vec3 getAmbientTextureColor(vec2& uv);
	vec3 getDiffuseTextureColor(vec2& uv);
	vec3 getSpecularTextureColor(vec2& uv);

	const vector<Vertex>& getVertices() const { return _vertices; }
	const vector<Vertex>& getVertices() { return const_cast<vector<Vertex>&>(static_cast<const Mesh &>(*this).getVertices()); }


private:


	void __triangulate(vector<Vertex> vertices, vector<unsigned int> indices);

	//    void __computeBoundingBox(vector<Vertex>& vertices);
	//    void __computeBoundingVolume(vector<Vertex>& vertices);

};


#endif
