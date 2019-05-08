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


typedef struct EnvMaps {

	vector<Image*> maps;
	bool refractiveMapping;
	GLfloat refractiveIndex; // This could be different from refractive Index of the mesh.

}EnvMaps;



typedef struct MeshTextures {

	Image *ambientTexture;
	Image *diffuseTexture;
	Image *specularTexture;
	Image *generalTexture;

	Image *normalsMap;

	EnvMaps *envMaps;

}MeshTextures ;





class Mesh : Object {

private:

	using super = Object;

	vector<Triangle*> _triangles;
//	vector<Vertex> _vertices;

	MeshTextures _textures;

//	Image* _meshAmbientTexture;
//	Image* _meshDiffuseTexture;
//	Image* _meshSpecularTexture;

//	vector<Image*> _envMaps;

	bool _envMapped = false;
	bool _refractiveMapping;
	GLfloat _envMapRefIndex;


public:


	Mesh(vector<Vertex>& vertices,
		 vector<GLuint>& indices,
		 ObjectProperties& properties,
		 MeshTextures& textures);
//		 Image *ambientTexture,
//		 Image *diffuseTexture,
//		 Image *specualrTexture,
//		 Image *generalTexture,
//		 EnvMaps &envMaps);

	virtual ~Mesh();

	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties);
	virtual bool intersectsRay(const Ray &r, GLfloat* dist, vec3* point, vec3* normal, ObjectTexColors* texColors, ObjectProperties* properties) const;


	vec3 getAmbientTextureColor(vec2& uv);
	vec3 getDiffuseTextureColor(vec2& uv);
	vec3 getSpecularTextureColor(vec2& uv);

	const vector<Triangle*>& getTriangles() const { return _triangles; }
	const vector<Triangle*>& getTriangles() { return const_cast<vector<Triangle*>&>(static_cast<const Mesh &>(*this).getTriangles()); }

//	const vector<Vertex>& getVertices() const { return _vertices; }
//	const vector<Vertex>& getVertices() { return const_cast<vector<Vertex>&>(static_cast<const Mesh &>(*this).getVertices()); }


	void print() const;

private:


	void __triangulate(vector<Vertex>& vertices, vector<GLuint>& indices);

};


#endif
